#include "timeline.h"

#include "raygui/raygui.h"

//define gui drop down list view
#include "raygui/gui_dropdown_listview.h"

//define timeline
#define GUI_3D_OBJECT_TIMELINE
#include "raygui/gui_timeline.h"

#include "raygui/gui_file_dialog.h"

#include <fstream> //for file read and write

#include "dialog_var.h"

#define MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT 200

Timeline::Timeline()
{
	showTimeline = false;
	
	addPointToTimeline = false;
	removePointFromTimeline = false;
	
	addPlaybackMarkerToTimeline = false;
	removePlaybackMarkerFromTimeline = false;
	
	time_frame_rate = 1;
	
	timeline_plots_position.reserve(25);
	
	//initialize default save data
	m_save_data.plots_save_data.reserve(25);
	
	m_save_data.number_of_plots = 0;
	
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
	
	//delete all remaining timeline plot playback markers
	while(timeline_plots_playback_markers.size() != 0)
	{
		delete timeline_plots_playback_markers.back().timeline_playback_markers;
		delete timeline_plots_playback_markers.back().timeline_settings_bool_array;
		timeline_plots_playback_markers.pop_back();
	}
	
}

void Timeline::Init(std::vector <std::unique_ptr <SoundProducer> > *sound_producer_vector, Listener* listener)
{
	main_listener_ptr = listener;
	sound_producer_vector_ref = sound_producer_vector;
	
	//add first timeline plot position for listener
	Timeline::AddPlotPositionToTimeline("Default");
	Timeline::AddPlotPlaybackMarkerToTimeline();
}



void Timeline::SetListenerInTimeline(Listener* listener_ptr){main_listener_ptr = listener_ptr;}

void Timeline::SetAddPointToTimelineBool(bool state){addPointToTimeline = state;}
void Timeline::SetRemovePointFromTimelineBool(bool state){removePointFromTimeline = state;}

void Timeline::SetAddPlaybackMarkerToTimelineBool(bool state){addPlaybackMarkerToTimeline = state;}
void Timeline::SetRemovePlaybackMarkerFromTimelineBool(bool state){removePlaybackMarkerFromTimeline = state;}

void Timeline::AddPlotPositionToTimeline(std::string name)
{
	timeline_plots_position.emplace_back( TimelinePlotPosition{ new float[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT],
																new float[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT],
																new float[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT], 
																new bool[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT] 
															}
										);
	
	for(size_t i = 0; i < MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT; i++)
	{
		timeline_plots_position.back().timeline_points_posx[i] = 0; 
		timeline_plots_position.back().timeline_points_posy[i] = 0; 
		timeline_plots_position.back().timeline_points_posz[i] = 0;

		timeline_plots_position.back().timeline_settings_bool_array[i] = false;
		timeline_plots_position.back().name = name;
	}
	
	m_save_data.number_of_plots += 1;
	
	m_save_data.plots_save_data.emplace_back(TimelinePlotPositionSaveData({0, name, ""}) );
}

void Timeline::RemovePlotPositionFromTimeline(size_t& index)
{
	//delete points
	delete timeline_plots_position[index].timeline_points_posx;
	delete timeline_plots_position[index].timeline_points_posy;
	delete timeline_plots_position[index].timeline_points_posz;
	
	std::swap(timeline_plots_position[index],timeline_plots_position.back());
	timeline_plots_position.pop_back();
	
	m_save_data.number_of_plots -= 1;
	
	std::swap(m_save_data.plots_save_data[index],m_save_data.plots_save_data.back());
	m_save_data.plots_save_data.pop_back();
}

void Timeline::AddPlotPlaybackMarkerToTimeline()
{
	timeline_plots_playback_markers.emplace_back( TimelinePlotPlaybackMarker{ new PlaybackMarkerType[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT]}
										);
	
	for(size_t i = 0; i < MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT; i++)
	{
		timeline_plots_playback_markers.back().timeline_playback_markers[i] = PlaybackMarkerType::NONE;

		timeline_plots_position.back().timeline_settings_bool_array[i] = false;
	}
}

void Timeline::RemovePlotPlaybackMarkerFromTimeline(size_t& index)
{
	delete timeline_plots_playback_markers[index].timeline_playback_markers;
	
	//switch out to be deleted with empty one at the end of vector, delete at end
	std::swap(timeline_plots_playback_markers[index],timeline_plots_playback_markers.back());
	timeline_plots_playback_markers.pop_back();
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

//counter variable for timeline
static uint32_t second_frame_count = 0;

static bool frameRateBoxEditMode = false;

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
		
		DrawFramesFileDialog();
				
	}
	
}

void Timeline::DrawTimelinePlotEditorGUI()
{
	
	//draw active object label
	GuiLabel((Rectangle){ 25, 520, 85, 30 }, "Active Object");
	
	//draw dropdown object editing box
	edit_obj_listview_activeIndex = Gui_Dropdown_ListView_Simple(&obj_dropdown_listview_settings, (Rectangle){ 100, 520, 70, 35 }, 
															obj_choices.c_str(), edit_obj_listview_itemsCount,
															edit_obj_listview_activeIndex
														);
												
	//Draw active timeline label
	GuiLabel((Rectangle){ 25, 450, 85, 30 }, "Active Timeline");
			
	//draw timeline position plot choices dropdown
	edit_timeline_listview_activeIndex = Gui_Dropdown_ListView_Simple(&timeline_dropdown_listview_settings, (Rectangle){ 25, 480, 70, 35 }, 
															timeline_choices.c_str(), edit_timeline_listview_itemsCount,
															edit_timeline_listview_activeIndex
														);
	
	
	//draw add button to add another timeline
	if( GuiButton( (Rectangle){ 125, 455, 20, 20 }, "+" ))
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
			//add timeline playback marker
			Timeline::AddPlotPlaybackMarkerToTimeline();
			
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
	if( GuiButton( (Rectangle){ 150, 455, 20, 20 }, "-" ) )
	{
		//remove timeline position
		size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
		//if index is more than 0 or valid
		if(index)
		{
			Timeline::RemovePlotPositionFromTimeline(index);
			Timeline::RemovePlotPlaybackMarkerFromTimeline(index);
		}
		
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
		
		m_save_data.plots_save_data[edit_timeline_listview_activeIndex].edit_index = edit_obj_listview_activeIndex;
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
			
			//std::cout << "edit index: " << edit_index << std::endl;
			
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
	
	//draw playback markers
	
	
}


void Timeline::DrawFramesGUI()
{
	//Draw frames label
	GuiLabel((Rectangle){ 25, 540, 85, 30 }, "Frames");
	
	//draw new frames button
	if( GuiButton( (Rectangle){ 25, 560, 40, 20 }, "New" ) )
	{
		//replace current file in use for frames for plot with empty filepath
		size_t edit_index = static_cast <size_t> (edit_timeline_listview_activeIndex);
		timeline_plots_position[edit_index].frames_filepath = "";
		
		//reset all points to false which erases it from GUI.
		for(uint16_t i = 0; i < 200; i++)
		{
			timeline_plots_position[edit_index].timeline_settings_bool_array[i] = false;
		}
	}
	
	//draw load frames button
	if( GuiButton( (Rectangle){ 25, 590, 40, 20 }, "Load" ) )
	{
		frames_file_state = FileFrameState::LOAD_NEW;
		fileDialogState.fileDialogActive = true; //activate file dialog
		global_dialog_in_use = true;
	}
	
	//draw save frames button
	if( GuiButton( (Rectangle){ 25, 620, 40, 20 }, "Save" ) )
	{
		frames_file_state = FileFrameState::SAVE_NEW;
		fileDialogState.fileDialogActive = true; //activate file dialog
		global_dialog_in_use = true;
	}
	
	//draw rate label
	GuiLabel((Rectangle){ 80, 580, 60, 30 }, "Frame Rate");
	
	//draw frame rate control
	if (GuiValueBox((Rectangle){ 80, 600, 60, 25 }, NULL, &time_frame_rate, 0, 100, frameRateBoxEditMode) )
	{
		frameRateBoxEditMode = !frameRateBoxEditMode;
		second_frame_count = 0; //reset just in case
	} 
}

void Timeline::DrawFramesFileDialog()
{
	//file operation logic
	if (fileDialogState.fileDialogActive){ GuiLock();}
	
	if(frames_file_state == FileFrameState::LOAD_NEW)
	{
		if (fileDialogState.SelectFilePressed)
		{			
			// Load project file (if supported extension)
			if (IsFileExtension(fileDialogState.fileNameText, ".bin") )
			{
				char projectFile[512] = { 0 };
				
				strcpy(projectFile, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
				std::string filepath = std::string(projectFile);
				
				std::cout << "load filepath for frames: " << filepath << std::endl;
				
				//load frames from file
				size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
				timeline_plots_position[index].frames_filepath = filepath;
				m_save_data.plots_save_data[index].frames_filepath = filepath;
				Timeline::LoadTimeFramesFromFile(filepath);
			}
			frames_file_state = FileFrameState::NONE;
			fileDialogState.SelectFilePressed = false;
			global_dialog_in_use = false;
		}
		
	}
	else if(frames_file_state == FileFrameState::SAVE_NEW)
	{
		if (fileDialogState.SelectFilePressed)
		{
			
			// save project file (if supported extension)
			
			//if file name was put in text box
			if (IsFileExtension(fileDialogState.fileNameTextBoxInputCopy, ".bin") )
			{
				char projectFile[512] = { 0 };
				
				strcpy(projectFile, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameTextBoxInputCopy));
				std::string filepath = std::string(projectFile);
				
				std::cout << "save filepath for frames: " << filepath << std::endl;
				
				//save frames to file
				size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
				timeline_plots_position[index].frames_filepath = filepath;
				m_save_data.plots_save_data[index].frames_filepath = filepath;
				Timeline::SaveTimeFramesToFile(filepath);
			}
			//else if file name was selected
			else if(IsFileExtension(fileDialogState.fileNameText, ".bin"))
			{
				char projectFile[512] = { 0 };
				
				strcpy(projectFile, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
				std::string filepath = std::string(projectFile);
				
				std::cout << "save filepath for frames: " << filepath << std::endl;
				
				//save frames to file
				size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
				timeline_plots_position[index].frames_filepath = filepath;
				m_save_data.plots_save_data[index].frames_filepath = filepath;
				Timeline::SaveTimeFramesToFile(filepath);
			}
			
			frames_file_state = FileFrameState::NONE;
			fileDialogState.SelectFilePressed = false;
			global_dialog_in_use = false;
		}
	}
	
	GuiUnlock();
	
	// call GuiFileDialog menu
	GuiFileDialog(&fileDialogState);
}



void Timeline::RunPlaybackWithTimeline()
{
	//set edit mode to false so that no editing happens during playback which can lead to errors
	timelineSettings.editMode = false;
	
	
	//increment timeline frame assuming constant 60 frames per second
	//increment number of frames based on time frame rate which is number of frames per second
	second_frame_count++;
	
	//if 1 second divided by time frame rate has passed
	//example: 60 frames_per_second / 3 time_frames_per_second = 20 frames_per_time_frame_second
	if(time_frame_rate == 0){return;}
	
	//for every time frame
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
			
			//get location
		
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
		
		//for every playback marker plot
		for(size_t i = 0; i < timeline_plots_playback_markers.size(); i++)
		{
			//if there is not a point at current timeline_frame, 
			//skip rest of loop code below and go to next iteration
			if(!timeline_plots_playback_markers[i].timeline_settings_bool_array[timelineSettings.current_timeline_frame])
			{
				continue;
			}
			
			//for sound bank index
		}
		
	}
	
}

void Timeline::ResumeEditModeInTimeline()
{
	timelineSettings.editMode = true;
	second_frame_count = 0;
}

//struct
struct TimeFramePositionData{
	uint16_t index;
	float x;
	float y;
	float z;
};

void Timeline::LoadSaveData(TimelineSaveData& save_data)
{
	m_save_data = save_data;
	
	//assuming m_save_data has been modified from loading from xml file
	timeline_plots_position.clear();
	timeline_plots_position.resize(save_data.number_of_plots);
	
	//for every timeline plot
	for(size_t i = 0; i < timeline_plots_position.size();i++)
	{
		//load name
		timeline_plots_position[i].name = save_data.plots_save_data[i].name;
		
		//load edit object index
		timeline_plots_position[i].indexObjectToEdit = save_data.plots_save_data[i].edit_index;
		
		//load frame filepath
		timeline_plots_position[i].frames_filepath = save_data.plots_save_data[i].frames_filepath;
		
		//initialize points of timeline frame
		timeline_plots_position[i].timeline_points_posx = new float[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT];
		timeline_plots_position[i].timeline_points_posy = new float[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT];
		timeline_plots_position[i].timeline_points_posz = new float[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT];
		timeline_plots_position[i].timeline_settings_bool_array = new bool[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT];
		for(size_t it = 0; it < MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT; it++)
		{
			timeline_plots_position[i].timeline_points_posx[it] = 0; 
			timeline_plots_position[i].timeline_points_posy[it] = 0; 
			timeline_plots_position[i].timeline_points_posz[it] = 0;

			timeline_plots_position[i].timeline_settings_bool_array[it] = false;
		}
		
		if(timeline_plots_position[i].frames_filepath != "")
		{
			//open file for reading
			std::ifstream infile (timeline_plots_position[i].frames_filepath,std::ifstream::binary | std::ifstream::in);
			
			if(!infile)
			{
				std::cout << "Unable to open file for reading: " << timeline_plots_position[i].frames_filepath << std::endl;
				continue;
			}
			
			
				
			//while have not reached end of file
			while( !infile.eof())
			{
				TimeFramePositionData data;
				
				//read point from file. index, x value, y value, z value
				infile.read(reinterpret_cast <char*>( &data) ,sizeof(data));
				
				//add point to timeline
				uint16_t arr_index = data.index;
				timeline_plots_position[i].timeline_points_posx[arr_index] = data.x;
				timeline_plots_position[i].timeline_points_posy[arr_index] = data.y;
				timeline_plots_position[i].timeline_points_posz[arr_index] = data.z;
				timeline_plots_position[i].timeline_settings_bool_array[arr_index] = true;
				
			}
				
			//close file
			infile.close();
		}
		
	}
	
	//change editor variables to match the newly loaded position
	edit_obj_listview_activeIndex = timeline_plots_position[edit_timeline_listview_activeIndex].indexObjectToEdit;
	obj_dropdown_listview_settings.scrollIndex = edit_obj_listview_activeIndex;
	Timeline::InitGUI();
}

TimelineSaveData& Timeline::GetSaveData(){return m_save_data;}

//saves timeline points to file
void Timeline::SaveTimeFramesToFile(std::string& filepath)
{
	//open file for writing 
	std::ofstream outfile (filepath,std::ofstream::binary | std::ofstream::out);
	
	if(!outfile)
	{
		std::cout << "Unable to open file for writing: " << filepath << std::endl;
	}
	
	size_t edit_index = static_cast <size_t> (edit_timeline_listview_activeIndex);
	
	//for current selected timeline
	for(uint16_t i = 0; i < MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT; i++)
	{
		//if there is a point added, add it to file
		if(timeline_plots_position[edit_index].timeline_settings_bool_array[i])
		{
			TimeFramePositionData data;
			
			data.index = i;
			data.x = timeline_plots_position[edit_index].timeline_points_posx[i];
			data.y = timeline_plots_position[edit_index].timeline_points_posy[i];
			data.z = timeline_plots_position[edit_index].timeline_points_posz[i];
			
			//write data
			outfile.write( reinterpret_cast <const char*>( &data) ,sizeof(data));
			
		}
	}
		
	//close file
	outfile.close();
	
}
	
//loads timeline points from file
void Timeline::LoadTimeFramesFromFile(std::string& filepath)
{
	
	//open file for reading
	std::ifstream infile (filepath,std::ifstream::binary | std::ifstream::in);
	
	if(!infile)
	{
		std::cout << "Unable to open file for reading: " << filepath << std::endl;
	}
	
	//for current selected timeline
	size_t edit_index = static_cast <size_t> (edit_timeline_listview_activeIndex);
		
	//while have not reached end of file
	while( !infile.eof())
	{
		TimeFramePositionData data;
		
		//read point from file. index, x value, y value, z value
		infile.read(reinterpret_cast <char*>( &data) ,sizeof(data));
		
		//add point to timeline
		uint16_t i = data.index;
		timeline_plots_position[edit_index].timeline_points_posx[i] = data.x;
		timeline_plots_position[edit_index].timeline_points_posy[i] = data.y;
		timeline_plots_position[edit_index].timeline_points_posz[i] = data.z;
		timeline_plots_position[edit_index].timeline_settings_bool_array[i] = true;
		
	}
		
	//close file
	infile.close();
}

void Timeline::HandleInput()
{
	if( IsKeyPressed(KEY_LEFT) )
	{
		//decrement frame
		if(timelineSettings.current_timeline_frame > 0)
		{
			timelineSettings.current_timeline_frame--;
		}
		
		timelineSettings.mouse_control = false;
	}
	else if( IsKeyPressed(KEY_RIGHT) )
	{
		//increment frame
		timelineSettings.current_timeline_frame++;
		timelineSettings.mouse_control = false;
	}
	
	if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		timelineSettings.mouse_control = true;
	}
	
	if(IsKeyPressed(KEY_ENTER))
	{
		timelineSettings.frameSelected = !timelineSettings.frameSelected;
	}
}
