#include "timeline.h"

#include "raygui/raygui.h"

//define gui drop down list view
#include "raygui/gui_dropdown_listview.h"

//define timeline
#define GUI_3D_OBJECT_TIMELINE
#include "raygui/gui_timeline.h"

#include "raygui/gui_file_dialog.h"

Timeline::Timeline()
{
	showTimeline = false;
	addPointToTimeline = false;
	removePointFromTimeline = false;
	
	time_frame_rate = 1;
}

Timeline::~Timeline()
{
	
	//delete all remaining timeline plot positions 
	while(timeline_plots_position.size() != 0)
	{
		delete timeline_plots_position.back().timeline_points_posx;
		delete timeline_plots_position.back().timeline_points_posy;
		delete timeline_plots_position.back().timeline_points_posz;
		delete timeline_plots_position.back().timeline_settings_bool_array;
		timeline_plots_position.pop_back();
	}
	
}

void Timeline::Init(std::vector <std::unique_ptr <SoundProducer> > *sound_producer_vector, Listener* listener)
{
	main_listener_ptr = listener;
	sound_producer_vector_ref = sound_producer_vector;
	
	//add first timeline plot position for listener
	Timeline::AddPlotPositionToTimeline("Default");
}



void Timeline::SetListenerInTimeline(Listener* listener_ptr){main_listener_ptr = listener_ptr;}

void Timeline::SetAddPointToTimelineBool(bool state){addPointToTimeline = state;}

void Timeline::SetRemovePointFromTimelineBool(bool state){removePointFromTimeline = state;}

void Timeline::AddPlotPositionToTimeline(std::string name)
{
	timeline_plots_position.emplace_back( TimelinePlotPosition{new float[200],new float[200],new float[200], new bool[200] });
	
	for(size_t i = 0; i < 200; i++)
	{
		timeline_plots_position.back().timeline_points_posx[i] = 0; 
		timeline_plots_position.back().timeline_points_posy[i] = 0; 
		timeline_plots_position.back().timeline_points_posz[i] = 0;

		timeline_plots_position.back().timeline_settings_bool_array[i] = false;
		timeline_plots_position.back().name = name;
	}
	
}

void Timeline::RemovePlotPositionFromTimeline(size_t& index)
{
	//delete points
	//increment by 1 because of listener
	delete timeline_plots_position[index].timeline_points_posx;
	delete timeline_plots_position[index].timeline_points_posy;
	delete timeline_plots_position[index].timeline_points_posz;
	
	std::swap(timeline_plots_position[index],timeline_plots_position.back());
	timeline_plots_position.pop_back();
}


void Timeline::SetShowTimelineBool(bool state){showTimeline = state;}

static int current_sound_producer_editing_index = -1;

void Timeline::SetObjectPicked(int index, ObjectType type)
{
	switch(type)
	{
		case ObjectType::NONE:{ m_final_edit_obj_index = -1; break;}
		case ObjectType::LISTENER:{ m_final_edit_obj_index = 0; current_sound_producer_editing_index = -1; break;}
		case ObjectType::SOUND_PRODUCER:
		{ 
			m_final_edit_obj_index = index + 1; 
			current_sound_producer_editing_index = index; 
			break;
		}
	}
	
}

void Timeline::SetTimeFrameRate(size_t rate){time_frame_rate = rate;}

static size_t max_num_frames = 200;

static TimelineSettings timelineSettings = InitTimelineSettings(max_num_frames);

static TimelineParameterSettings positionTimelineSettings = InitTimelineParameterSettings(max_num_frames,nullptr,200,440);


//dropdown list view for timeline being edited
static DropDownListViewSettings timeline_dropdown_listview_settings = InitDropDownListViewSettings(3,false,false,0);
static int edit_timeline_listview_activeIndex = 0;
static int edit_timeline_listview_itemsCount = 0;
static std::string timeline_choices = "";

//dropdown list view for object being edited.
//DropDownListViewSettings InitDropDownListViewSettings(int itemsShowCount, bool editMode, bool valueChanged, int scrollIndex)
static DropDownListViewSettings obj_dropdown_listview_settings = InitDropDownListViewSettings(3,false,false,0);
static int edit_obj_listview_activeIndex = 0;
static int edit_obj_listview_itemsCount = 0;
static std::string obj_choices = "";

//variables for text input of timeline
static bool addTimeline = false;
static char textInput[256] = { 0 };

//variables for file save/load
static GuiFileDialogState fileDialogState = InitGuiFileDialog(420, 310, GetWorkingDirectory(), false);


enum class FileFrameState : std::uint8_t{NONE=0, SAVE_NEW,LOAD_NEW};
static FileFrameState frames_file_state = FileFrameState::NONE;

void Timeline::InitGUI()
{
	obj_choices = "";
	
	if(sound_producer_vector_ref)
	{
		obj_choices = "None;Listener;";
		
		for(size_t i = 0; i < sound_producer_vector_ref->size(); i++)
		{
			obj_choices += sound_producer_vector_ref->at(i)->GetNameString() + ";";
		}
		
		edit_obj_listview_itemsCount = (int)sound_producer_vector_ref->size() + 2;
				
	}
	
	timeline_choices = "";
	
	for(size_t i = 0; i < timeline_plots_position.size(); i++)
	{
		timeline_choices += timeline_plots_position[i].name + ";";
	}
	
	edit_timeline_listview_itemsCount = (int)timeline_plots_position.size();
	
	fileDialogState.position = {200,200};
}

void Timeline::DrawGui_Item()
{
	//draw timeline
	
	if( showTimeline )
	{
		//draw timeline area
		Rectangle drawAreaRect = {0, 400 , GetScreenWidth(), GetScreenHeight()};
		DrawRectangleRec(drawAreaRect, Fade(GRAY, 0.5f));
		
		DrawFramesGUI();
		
		DrawTimelinePointsGUI();
		
		DrawTimelinePlotEditorGUI();
		
		
	}
	
}

void Timeline::DrawTimelinePlotEditorGUI()
{
	//draw dropdown object editing box
	edit_obj_listview_activeIndex = Gui_Dropdown_ListView_Simple(&obj_dropdown_listview_settings, (Rectangle){ 100, 480, 70, 45 }, 
															obj_choices.c_str(), edit_obj_listview_itemsCount,
															edit_obj_listview_activeIndex
														);
														
	//draw timeline position plot choices dropdown
	edit_timeline_listview_activeIndex = Gui_Dropdown_ListView_Simple(&timeline_dropdown_listview_settings, (Rectangle){ 100, 420, 70, 45 }, 
															timeline_choices.c_str(), edit_timeline_listview_itemsCount,
															edit_timeline_listview_activeIndex
														);
														
	//draw add button to add another timeline
	if( GuiButton( (Rectangle){ 25, 490, 70, 30 }, GuiIconText(0, "Add") ))
	{
		addTimeline = true;
	}
	
	if(addTimeline)
	{
		//prompt for timeline name
		
		
		int result = GuiTextInputBox((Rectangle){ GetScreenWidth()/2 - 120, GetScreenHeight()/2 - 60, 240, 140 }, GuiIconText(0, "Timeline Name Input..."), "Give the new timeline a name.\nCancel to stop creating timeline.", "Ok;Cancel", textInput);
		
		//if ok clicked
		if (result == 1)
		{
			addTimeline = false;
			
			//add timeline position and its name
			Timeline::AddPlotPositionToTimeline(std::string(textInput));
			
		}
		//else if cancel clicked
		else if(result == 2)
		{
			addTimeline = false;
		}

		if ((result == 0) || (result == 1) || (result == 2))
		{
			strcpy(textInput, "\0");
		}
	}
	
	//draw remove button to remove current edited timeline
	if( GuiButton( (Rectangle){ 25, 520, 70, 30 }, GuiIconText(0, "Remove") ) )
	{
		//remove timeline position
		size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
		Timeline::RemovePlotPositionFromTimeline(index);
	}
	
	//if timeline choice edited has changed
	if(timeline_dropdown_listview_settings.valueChanged)
	{
		timeline_dropdown_listview_settings.valueChanged = false;
		
		timeline_dropdown_listview_settings.scrollIndex = edit_timeline_listview_activeIndex;
		
		edit_obj_listview_activeIndex = timeline_plots_position[edit_timeline_listview_activeIndex].indexObjectToEdit;
		
		obj_dropdown_listview_settings.valueChanged = true;
	}
	
	//if object choice edited by timeline changes													
	if(obj_dropdown_listview_settings.valueChanged )
	{
		obj_dropdown_listview_settings.valueChanged = false;
		
		obj_dropdown_listview_settings.scrollIndex = edit_obj_listview_activeIndex;
		
		timeline_plots_position[edit_timeline_listview_activeIndex].indexObjectToEdit = edit_obj_listview_activeIndex;
	}
	
	
	if(edit_timeline_listview_activeIndex >= 0)
	{
		positionTimelineSettings.array_points_ptr = timeline_plots_position[edit_timeline_listview_activeIndex].timeline_settings_bool_array;
	}
	else
	{
		positionTimelineSettings.array_points_ptr = nullptr;
	}
}

void Timeline::DrawTimelinePointsGUI()
{
	float leftBound = 200;
	timelineSettings.mouseArea = {leftBound, 400 , GetScreenWidth(), GetScreenHeight()};
		
	DrawRectangleRec(timelineSettings.mouseArea, Fade(GRAY, 0.5f));

	Gui_Timeline(&timelineSettings);
		
		
	//draw position timeline if there are points to draw
	if(positionTimelineSettings.array_points_ptr )
	{
		
		//if adding point to timeline
		if(addPointToTimeline && timelineSettings.frameSelected)
		{
			bool addPoint = false;
			float x,y,z;
			
			int edit_index = timeline_plots_position[edit_timeline_listview_activeIndex].indexObjectToEdit;
			
			std::cout << "edit index: " << edit_index << std::endl;
			
			//if listener
			if(edit_index == 1)
			{
				//get position of listener
				x = main_listener_ptr->getPositionX();
				y = main_listener_ptr->getPositionY();
				z = main_listener_ptr->getPositionZ();
				addPoint = true;
				
			}
			//else if sound producer
			else if(edit_index >= 2)
			{
				//get position of sound	producer
				
				x = sound_producer_vector_ref->at(edit_index - 2)->GetPositionX();
				y = sound_producer_vector_ref->at(edit_index - 2)->GetPositionY();
				z = sound_producer_vector_ref->at(edit_index - 2)->GetPositionZ();
				
				addPoint = true;
				
			}
			else
			{
				//do nothing
				addPoint = false;
			}
			
			if(addPoint)
			{
				//add point to graphical timeline depending on frame selected
				timeline_plots_position[edit_timeline_listview_activeIndex].timeline_settings_bool_array[timelineSettings.current_timeline_frame] = true;
				
				//add point to position timeline
				timeline_plots_position[edit_timeline_listview_activeIndex].timeline_points_posx[timelineSettings.current_timeline_frame] = x; 
				timeline_plots_position[edit_timeline_listview_activeIndex].timeline_points_posy[timelineSettings.current_timeline_frame] = y; 
				timeline_plots_position[edit_timeline_listview_activeIndex].timeline_points_posz[timelineSettings.current_timeline_frame] = z;
			}
			
			addPointToTimeline = false;
		}
		else if(addPointToTimeline)
		{
			addPointToTimeline = false;
		}		
		
		if(removePointFromTimeline && timelineSettings.frameSelected)
		{
			
			//remove point from graphical timeline depending on frame selected
			timeline_plots_position[edit_timeline_listview_activeIndex].timeline_settings_bool_array[timelineSettings.current_timeline_frame] = false;
			
			//remove point from position timeline with reset
			timeline_plots_position[edit_timeline_listview_activeIndex].timeline_points_posx[timelineSettings.current_timeline_frame] = 0; 
			timeline_plots_position[edit_timeline_listview_activeIndex].timeline_points_posy[timelineSettings.current_timeline_frame] = 0; 
			timeline_plots_position[edit_timeline_listview_activeIndex].timeline_points_posz[timelineSettings.current_timeline_frame] = 0;
			
			removePointFromTimeline = false;
		}
		else if(removePointFromTimeline)
		{
			removePointFromTimeline = false;
		}
			
		Gui_Timeline_Parameter(&positionTimelineSettings);		
	}
	
}

void Timeline::DrawFramesGUI()
{
	//draw new frames button
	if( GuiButton( (Rectangle){ 25, 560, 85, 30 }, GuiIconText(0, "New Frames") ) )
	{
		//replace current file in use for frames for plot with empty filepath
		size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
		timeline_plots_position[index].frames_filepath = "";
	}
	
	//draw load frames button
	if( GuiButton( (Rectangle){ 25, 600, 85, 30 }, GuiIconText(0, "Load Frames") ) )
	{
		frames_file_state = FileFrameState::LOAD_NEW;
		fileDialogState.fileDialogActive = true; //activate file dialog
	}
	
	//draw save frames button
	if( GuiButton( (Rectangle){ 115, 600, 85, 30 }, GuiIconText(0, "Save Frames") ) )
	{
		frames_file_state = FileFrameState::SAVE_NEW;
		fileDialogState.fileDialogActive = true; //activate file dialog
	}
	
	
	//file operation logic
	if (fileDialogState.fileDialogActive){ GuiLock();}
	
	if(frames_file_state == FileFrameState::LOAD_NEW)
	{
		if (fileDialogState.SelectFilePressed)
		{			
			// Load project file (if supported extension)
			if (IsFileExtension(fileDialogState.fileNameText, ".xml") )
			{
				char projectFile[512] = { 0 };
				
				strcpy(projectFile, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
				std::string filepath = std::string(projectFile);
				
				std::cout << "load filepath for frames: " << filepath << std::endl;
				
				//load frames from file
				size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
				timeline_plots_position[index].frames_filepath = filepath;
			}
			frames_file_state = FileFrameState::NONE;
			fileDialogState.SelectFilePressed = false;
		}
		
	}
	else if(frames_file_state == FileFrameState::SAVE_NEW)
	{
		if (fileDialogState.SelectFilePressed)
		{
			
			// save project file (if supported extension)
			
			//if file name was put in text box
			if (IsFileExtension(fileDialogState.fileNameTextBoxInputCopy, ".txt") )
			{
				char projectFile[512] = { 0 };
				
				strcpy(projectFile, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameTextBoxInputCopy));
				std::string filepath = std::string(projectFile);
				
				std::cout << "save filepath for frames: " << filepath << std::endl;
				
				//save frames to file
				size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
				timeline_plots_position[index].frames_filepath = filepath;
			}
			//else if file name was selected
			else if(IsFileExtension(fileDialogState.fileNameText, ".txt"))
			{
				char projectFile[512] = { 0 };
				
				strcpy(projectFile, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
				std::string filepath = std::string(projectFile);
				
				std::cout << "save filepath for frames: " << filepath << std::endl;
				
				//save frames to file
				size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
				timeline_plots_position[index].frames_filepath = filepath;
			}
			
			frames_file_state = FileFrameState::NONE;
			fileDialogState.SelectFilePressed = false;
		}
	}
	
	GuiUnlock();
	
	// call GuiFileDialog menu
	GuiFileDialog(&fileDialogState);
}

static uint32_t second_frame_count = 0;

void Timeline::RunPlaybackWithTimeline()
{
	//set edit mode to false so that 
	timelineSettings.editMode = false;
	
	
	//increment timeline frame assuming 60 frames per second
	//increment number of frames based on time frame rate which is number of frames per second
	second_frame_count++;
	
	//if 1 second divided by time frame rate has passed
	//example: 60 frames_per_second / 3 time_frames_per_second = 20 frames_per_time_frame_second
	if(second_frame_count == 60 / time_frame_rate)
	{
		
		timelineSettings.current_timeline_frame++;
		second_frame_count = 0;
		
		
		//for every position plot
		for(size_t i = 0; i < timeline_plots_position.size(); i++)
		{
			//if there is not a point at current timeline_frame, 
			//skip rest of loop code below and go to next iteration
			if(!timeline_plots_position[i].timeline_settings_bool_array[timelineSettings.current_timeline_frame])
			{
				continue;
			}
			
			//get location in timeline
		
			float& x = timeline_plots_position[i].timeline_points_posx[timelineSettings.current_timeline_frame]; 
			float& y = timeline_plots_position[i].timeline_points_posy[timelineSettings.current_timeline_frame]; 
			float& z = timeline_plots_position[i].timeline_points_posz[timelineSettings.current_timeline_frame];
			
			//if listener
			if(timeline_plots_position[i].indexObjectToEdit == 1)
			{
				main_listener_ptr->setPositionX(x);
				main_listener_ptr->setPositionY(y);
				main_listener_ptr->setPositionZ(z);
			}
			//else if sound producer
			else if(timeline_plots_position[i].indexObjectToEdit >= 2 && timeline_plots_position[i].indexObjectToEdit - 2 < sound_producer_vector_ref->size())
			{
				sound_producer_vector_ref->at(timeline_plots_position[i].indexObjectToEdit - 2)->SetPositionX(x);
				sound_producer_vector_ref->at(timeline_plots_position[i].indexObjectToEdit - 2)->SetPositionY(y);
				sound_producer_vector_ref->at(timeline_plots_position[i].indexObjectToEdit - 2)->SetPositionZ(z);
			}
		}
			
			
		
	}
	
}

void Timeline::ResumeEditModeInTimeline()
{
	timelineSettings.editMode = true;
}

//saves timeline points to file
void Timeline::SaveTimeFramesToFile(std::string filepath)
{
	//open file for writing 
	
	//for current selected timeline
		//if there is a point added, add it to file
	
	//close file
}
	
//loads timeline points from file
void Timeline::LoadTimeFramesFromFile(std::string filepath)
{
	//open file for reading
	
	//for current selected timeline
		//read point from file. index, x value, y value, z value
	
	//close file
}

const TimelineSaveData* Timeline::GetPointerToTimelineSaveData(){return &m_save_data;}
