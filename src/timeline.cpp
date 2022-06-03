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

#define MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT 600

//timeline settings

static size_t max_num_frames = MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT;

static int max_num_frames_to_display = 200;
static int timeline_leftmost_frame = 0;
static int timeline_rightmost_frame = max_num_frames_to_display;

static TimelineSettings timelineSettings = InitTimelineSettings(max_num_frames,&timeline_leftmost_frame,&timeline_rightmost_frame,max_num_frames_to_display);

static TimelineParameterSettings positionTimelineSettings = InitTimelineParameterSettings(max_num_frames,nullptr,200,440,
																							&timeline_leftmost_frame,&timeline_rightmost_frame,max_num_frames_to_display);

static TimelineParameterSettings playbackMarkerTimelineSettings = InitTimelineParameterSettings(max_num_frames,nullptr,200,440,
																								&timeline_leftmost_frame,&timeline_rightmost_frame,max_num_frames_to_display);


Timeline::Timeline()
{
	showTimeline = false;
	
	addPointToTimeline = false;
	removePointFromTimeline = false;
	
	addPlaybackMarkerToTimeline = false;
	removePlaybackMarkerFromTimeline = false;
	
	showTimelineParameterPropertiesBox = false;
	
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
	
	//set array pointers to point to first timeline
	positionTimelineSettings.array_points_ptr = timeline_plots_position[0].timeline_settings_bool_array;
	playbackMarkerTimelineSettings.array_points_ptr = timeline_plots_playback_markers[0].timeline_settings_bool_array;
	playbackMarkerTimelineSettings.draw_color = RED;
}



void Timeline::SetListenerInTimeline(Listener* listener_ptr){main_listener_ptr = listener_ptr;}

void Timeline::SetAddPointToTimelineBool(bool state){addPointToTimeline = state;}
void Timeline::SetRemovePointFromTimelineBool(bool state){removePointFromTimeline = state;}

void Timeline::SetAddPlaybackMarkerToTimelineBool(bool state, PlaybackMarkerType type){addPlaybackMarkerToTimeline = state; addPlaybackMarkerToTimeline_type = type;}
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
	timeline_plots_playback_markers.emplace_back( TimelinePlotPlaybackMarker{ new PlaybackMarkerType[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT], 
													new bool[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT] }
										);
	
	for(size_t i = 0; i < MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT; i++)
	{
		timeline_plots_playback_markers.back().timeline_playback_markers[i] = PlaybackMarkerType::NONE;

		timeline_plots_playback_markers.back().timeline_settings_bool_array[i] = false;
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
		
		timeline_plots_playback_markers[edit_timeline_listview_activeIndex].indexObjectToEdit = edit_obj_listview_activeIndex;
		
		m_save_data.plots_save_data[edit_timeline_listview_activeIndex].edit_index = edit_obj_listview_activeIndex;
	}
	
	
	if(edit_timeline_listview_activeIndex >= 0)
	{
		positionTimelineSettings.array_points_ptr = timeline_plots_position[edit_timeline_listview_activeIndex].timeline_settings_bool_array;
		playbackMarkerTimelineSettings.array_points_ptr = timeline_plots_playback_markers[edit_timeline_listview_activeIndex].timeline_settings_bool_array;
	}
	else
	{
		positionTimelineSettings.array_points_ptr = nullptr;
		playbackMarkerTimelineSettings.array_points_ptr = nullptr;
	}
}

void Timeline::ShowPropertiesBox(int x, int y, int timeline_index, int timeline_current_frame)
{
	//skip if timeline current frame is more than max number of points in timeline plot
	if(timeline_current_frame >= MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT){return;}
	
	Rectangle draw_rect = (Rectangle){x,y,200,100};
	Rectangle rect_msg = (Rectangle){x,y,200,40};
	GuiPanel(draw_rect);
	
	std::string playback_marker_str = "";
	
	//if there is playback marker info
	if(timeline_plots_playback_markers[timeline_index].timeline_settings_bool_array[timeline_current_frame])
	{
		playback_marker_str = "Playback Marker: ";
		PlaybackMarkerType pm_type = timeline_plots_playback_markers[timeline_index].timeline_playback_markers[timeline_current_frame];
	
		switch(pm_type)
		{
			case PlaybackMarkerType::NONE:{ playback_marker_str += "None"; break;}
			case PlaybackMarkerType::START_PLAY:{ playback_marker_str += "Start Play"; break;}
			case PlaybackMarkerType::PAUSE:{ playback_marker_str += "Pause"; break;}
			case PlaybackMarkerType::RESUME:{ playback_marker_str += "Resume"; break;}
			case PlaybackMarkerType::END_PLAY:{ playback_marker_str += "End Play"; break;}
		}
	}
	
	std::string position_str = "";
	
	//if there is timeline plot position info
	if(timeline_plots_position[timeline_index].timeline_settings_bool_array[timeline_current_frame])
	{
		float pos_x = timeline_plots_position[timeline_index].timeline_points_posx[timeline_current_frame];
		float pos_y = timeline_plots_position[timeline_index].timeline_points_posy[timeline_current_frame];
		float pos_z = timeline_plots_position[timeline_index].timeline_points_posz[timeline_current_frame];
		
		position_str = "position(x,y,z): \n" + std::to_string(pos_x) + " " + std::to_string(pos_y) + " " + std::to_string(pos_z);
	}
	
	std::string timeframe_str = "Timeline Frame: " + std::to_string(timeline_current_frame);
	
	std::string message = timeframe_str + "\n" + position_str + "\n" + playback_marker_str;
							
	GuiLabel( rect_msg, message.c_str() );
	
}

void Timeline::DrawTimelinePointsGUI()
{
	float leftBound = 200;
	float upperBound = 400;
	timelineSettings.mouseArea = {leftBound, upperBound , GetScreenWidth(), GetScreenHeight()};
		
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
	if(playbackMarkerTimelineSettings.array_points_ptr)
	{
		//if adding playback marker to timeline
		if(addPlaybackMarkerToTimeline && addPlaybackMarkerToTimeline_type != PlaybackMarkerType::NONE 
			&& timelineSettings.frameSelected)
		{
			bool addMarker = false;
			
			int edit_index = timeline_plots_playback_markers[edit_timeline_listview_activeIndex].indexObjectToEdit;
			
			//std::cout << "edit index: " << edit_index << std::endl;
			
			//if sound producer
			if(edit_index >= 2)
			{
				//get position of sound	producer
				
				addMarker = true;
				
			}
			else
			{
				//do nothing
				addMarker = false;
			}
			
			if(addMarker)
			{
				//add marker to graphical timeline depending on frame selected
				timeline_plots_playback_markers[edit_timeline_listview_activeIndex].timeline_settings_bool_array[timelineSettings.current_timeline_frame] = true;
				
				//add marker to timeline plot data
				timeline_plots_playback_markers[edit_timeline_listview_activeIndex].timeline_playback_markers[timelineSettings.current_timeline_frame] = addPlaybackMarkerToTimeline_type;
			}
			
			addPlaybackMarkerToTimeline = false;
			addPlaybackMarkerToTimeline_type = PlaybackMarkerType::NONE;
		}
		else if(addPlaybackMarkerToTimeline)
		{
			addPlaybackMarkerToTimeline = false;
		}		
		
		if(removePlaybackMarkerFromTimeline && timelineSettings.frameSelected)
		{
			
			//remove point from graphical timeline depending on frame selected
			timeline_plots_playback_markers[edit_timeline_listview_activeIndex].timeline_settings_bool_array[timelineSettings.current_timeline_frame] = false;
			
			//remove point from position timeline with reset
			timeline_plots_playback_markers[edit_timeline_listview_activeIndex].timeline_playback_markers[timelineSettings.current_timeline_frame] = PlaybackMarkerType::NONE;
			
			removePlaybackMarkerFromTimeline = false;
		}
		else if(removePlaybackMarkerFromTimeline)
		{
			removePlaybackMarkerFromTimeline = false;
		}
			
		Gui_Timeline_Parameter(&playbackMarkerTimelineSettings);
	}
	
	//if current time frame is valid and selected
	if(timelineSettings.current_timeline_frame >= 0 && timelineSettings.frameSelected 
		&& showTimelineParameterPropertiesBox)
	{
		//show properties in a box
		//show to the left of timeline marker 
		Timeline::ShowPropertiesBox(leftBound,upperBound + 100,edit_timeline_listview_activeIndex,timelineSettings.current_timeline_frame);
	}
	
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
		for(uint16_t i = 0; i < MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT; i++)
		{
			timeline_plots_position[edit_index].timeline_settings_bool_array[i] = false;
			timeline_plots_playback_markers[edit_index].timeline_settings_bool_array[i] = false;
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
		//keep directory path set to the data directory for timeline data
		strcpy(fileDialogState.dirPathText,m_data_dir_path.c_str());
		fileDialogState.dirPathEditMode = false;
		
		if (fileDialogState.SelectFilePressed)
		{			
			// Load frames file (if supported extension)
			if (IsFileExtension(fileDialogState.fileNameText, ".bin") )
			{
				std::string filepath = std::string(fileDialogState.fileNameText);
				
				//std::cout << "load filepath for frames: " << filepath << std::endl;
				
				//load frames from file
				size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
				//only save filename
				timeline_plots_position[index].frames_filepath = filepath;
				m_save_data.plots_save_data[index].frames_filepath = filepath;
				
				std::string full_filepath = m_data_dir_path + filepath;
				Timeline::LoadTimeFramesFromFile(full_filepath);
			}
			frames_file_state = FileFrameState::NONE;
			fileDialogState.SelectFilePressed = false;
			global_dialog_in_use = false;
		}
		
	}
	else if(frames_file_state == FileFrameState::SAVE_NEW)
	{
		//keep directory path set to the data directory for timeline data
		strcpy(fileDialogState.dirPathText,m_data_dir_path.c_str());
		fileDialogState.dirPathEditMode = false;
		
		
		if (fileDialogState.SelectFilePressed)
		{
			
			// save project file (if supported extension)
			std::string filename = std::string(fileDialogState.fileNameText);
			
			//if file was not chosen from list
			if (filename.empty())
			{
				
				std::string filepath = std::string(fileDialogState.fileNameTextBoxInputCopy);
				
				//if .bin is not in ending of file name
				if(filepath.substr(filepath.length() - 4,filepath.length() - 1) != ".bin")
				{
					filepath.append(".bin");
				}
				
				//std::cout << "save filename for frames: " << filepath << std::endl;
				
				//save frames to file
				size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
				//only save file name
				timeline_plots_position[index].frames_filepath = filepath; 
				m_save_data.plots_save_data[index].frames_filepath = filepath;
				
				std::string full_filepath = m_data_dir_path + filepath;
				Timeline::SaveTimeFramesToFile(full_filepath);
			}
			//else if file name was selected and has file extension .bin
			else
			{
				if(!IsFileExtension(fileDialogState.fileNameText, ".bin"))
				{
					//do nothing
				}
				else
				{
					std::string filepath = std::string(fileDialogState.fileNameText);
				
					//std::cout << "save filename for frames: " << filepath << std::endl;
					
					//save frames to file
					size_t index = static_cast <size_t> (edit_timeline_listview_activeIndex);
					//only save filename
					timeline_plots_position[index].frames_filepath = filepath;
					m_save_data.plots_save_data[index].frames_filepath = filepath;
					
					std::string full_filepath = m_data_dir_path + filepath;
					Timeline::SaveTimeFramesToFile(full_filepath);
				}
				
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

static PlaybackMarkerTraveler playback_marker_traveler;

void Timeline::SolveAudioPlaybackInTimeline(ImmediateModeSoundPlayer* im_sound_player_ptr)
{
	
	float progressValue = 0.0f;
	
	float progressValueIncrement = 1 / timeline_plots_playback_markers.size();
	
	//for every audio playback marker plot
	for(size_t i = 0; i < timeline_plots_playback_markers.size(); i++)
	{
		//if object edited by playback marker plot is not a sound producer , skip
		bool is_sound_producer = false;
		
		//skip playback marker plot if not sound producer
		if(timeline_plots_playback_markers[i].indexObjectToEdit >= 2 && timeline_plots_playback_markers[i].indexObjectToEdit - 2 < sound_producer_vector_ref->size())
		{
			is_sound_producer = true;
			if(!is_sound_producer){continue;}
		}
		
		//check from zero to current timeline frame
		for(size_t it_frame = 0; it_frame < timelineSettings.current_timeline_frame; it_frame++)
		{
			//skip frame if there is no playback marker
			if(!timeline_plots_playback_markers[i].timeline_settings_bool_array[it_frame])
			{
				continue;
			}
			
			//determine current playback time for sound producer
			PlaybackMarkerType& pm_type = timeline_plots_playback_markers[i].timeline_playback_markers[it_frame];
			playback_marker_traveler.RunTravelerBasedOnPlaybackMarkerType(pm_type);
		}
		
		//if time recorded in playback marker is not zero
		if(playback_marker_traveler.GetCurrentTime() > 0.0f)
		{
			int buffer_player_index = timeline_plots_playback_markers[i].indexObjectToEdit - 2;
			double& current_time = playback_marker_traveler.GetCurrentTime();
			
			//forward audio playback of sound producer
			im_sound_player_ptr->SetCurrentTimeInBuffer_ComplexPlayback(buffer_player_index,current_time);
			
			//set play state depending on playback marker traveler state
			switch(playback_marker_traveler.GetCurrentState())
			{
				case PlaybackMarkerTraveler::TravelerState::FORWARD_TIME:
				{
					im_sound_player_ptr->SetBufferPlayerToPlay_ComplexPlayback(buffer_player_index); 
					break;
				}
				case PlaybackMarkerTraveler::TravelerState::PAUSED:
				{
					im_sound_player_ptr->SetBufferPlayerToPause_ComplexPlayback(buffer_player_index);
					break;
				}
				case PlaybackMarkerTraveler::TravelerState::NONE:
				{
					break;
				}
			}
			
		}
		
		//show progress bar indicating status of solving of audio playback
		GuiProgressBar((Rectangle){ 320, 460, 200, 20 }, NULL, NULL, progressValue, 0, 1);
		
		//update progress bar after finishing checking one playback marker plot
		progressValue += progressValueIncrement;
		
		//reset playback marker traveler
		playback_marker_traveler.ResetTraveler();
	}
}


void Timeline::RunPlaybackWithTimeline(ImmediateModeSoundPlayer* im_sound_player_ptr)
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
			
			//if object edited is sound producer
			if(timeline_plots_playback_markers[i].indexObjectToEdit >= 2 && timeline_plots_playback_markers[i].indexObjectToEdit - 2 < sound_producer_vector_ref->size())
			{
				//buffer player index is linked to sound producer in im sound player
				int buffer_player_index = timeline_plots_position[i].indexObjectToEdit - 2;
				
				switch(timeline_plots_playback_markers[i].timeline_playback_markers[timelineSettings.current_timeline_frame])
				{
					case PlaybackMarkerType::NONE:{ break;}
					case PlaybackMarkerType::START_PLAY:{im_sound_player_ptr->SetBufferPlayerToPlay_ComplexPlayback(buffer_player_index); break;}
					case PlaybackMarkerType::RESUME:{im_sound_player_ptr->SetBufferPlayerToPlay_ComplexPlayback(buffer_player_index); break;}
					case PlaybackMarkerType::PAUSE:{im_sound_player_ptr->SetBufferPlayerToPause_ComplexPlayback(buffer_player_index); break;}
					case PlaybackMarkerType::END_PLAY:{im_sound_player_ptr->SetBufferPlayerToStop_ComplexPlayback(buffer_player_index); break;}
				}				
			}
		}
		
	}
	
}

void Timeline::ResumeEditModeInTimeline()
{
	timelineSettings.editMode = true;
	second_frame_count = 0;
}

//struct
struct TimeFramePositionPlaybackData{
	uint16_t index;
	uint8_t position_exist;
	float x;
	float y;
	float z;
	uint8_t playback_marker_exist;
	uint8_t playback_marker_type;
};

// IMPORTANT NOTE
// 3d audio producer saves and loads binary data in little endian 
// if system is big endian, then, binary data is converted to big endian
// otherwise if system is little endian, binary data is not converted

static float ReverseFloat( const float inFloat )
{
   float retVal;
   char *floatToConvert = ( char* ) & inFloat;
   char *returnFloat = ( char* ) & retVal;

   // swap the bytes into a temporary buffer
   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return retVal;
}

static uint16_t ReverseInt_16bit(const uint16_t val)
{
	return (val << 8) | (val >> 8 );
}

static bool IsSystemBigEndian()
{
    uint16_t word = 1; // 0x0001
    uint8_t *first_byte = (uint8_t*) &word; // points to the first byte of word
    return !(*first_byte); // true if the first byte is zero
}

void Timeline::LoadSaveData(TimelineSaveData& save_data)
{
	m_save_data = save_data;
	
	//assuming m_save_data has been modified from loading from xml file
	timeline_plots_position.clear();
	timeline_plots_playback_markers.clear();
	
	timeline_plots_position.resize(save_data.number_of_plots);
	timeline_plots_playback_markers.resize(save_data.number_of_plots);
	
	//check if system is big endian
	bool systemIsBigEndian = IsSystemBigEndian();
	
	//for every timeline plot
	for(size_t i = 0; i < timeline_plots_position.size();i++)
	{
		//load name
		timeline_plots_position[i].name = save_data.plots_save_data[i].name;
		
		//load edit object index
		timeline_plots_position[i].indexObjectToEdit = save_data.plots_save_data[i].edit_index;
		timeline_plots_playback_markers[i].indexObjectToEdit = save_data.plots_save_data[i].edit_index;
		
		//load frame filepath
		timeline_plots_position[i].frames_filepath = save_data.plots_save_data[i].frames_filepath;
		
		//initialize points of timeline frame
		timeline_plots_position[i].timeline_points_posx = new float[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT];
		timeline_plots_position[i].timeline_points_posy = new float[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT];
		timeline_plots_position[i].timeline_points_posz = new float[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT];
		timeline_plots_position[i].timeline_settings_bool_array = new bool[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT];
		
		//initialize playback markers of timeline frame
		timeline_plots_playback_markers[i].timeline_playback_markers = new PlaybackMarkerType[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT];
		timeline_plots_playback_markers[i].timeline_settings_bool_array = new bool[MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT];
		
		for(size_t it = 0; it < MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT; it++)
		{
			timeline_plots_position[i].timeline_points_posx[it] = 0; 
			timeline_plots_position[i].timeline_points_posy[it] = 0; 
			timeline_plots_position[i].timeline_points_posz[it] = 0;

			timeline_plots_position[i].timeline_settings_bool_array[it] = false;
			
			timeline_plots_playback_markers[i].timeline_playback_markers[it] = PlaybackMarkerType::NONE;
			
			timeline_plots_playback_markers[i].timeline_settings_bool_array[it] = false;
		}
		
		if(timeline_plots_position[i].frames_filepath != "")
		{
			//open file for reading
			std::string full_frame_path = m_data_dir_path + timeline_plots_position[i].frames_filepath; 
			std::ifstream infile (full_frame_path,std::ifstream::binary | std::ifstream::in);
			
			if(!infile)
			{
				std::cout << "Unable to open file for reading: " << full_frame_path << std::endl;
				continue;
			}
			
			
			
			//if system is not big endian, assuming this is little endian
			if(!systemIsBigEndian)
			{
				//while have not reached end of file
				while( !infile.eof())
				{
					
					TimeFramePositionPlaybackData data;
					
					//read point from file. index, x value, y value, z value
					infile.read(reinterpret_cast <char*>( &data) ,sizeof(data));
					
					//add point to timeline
					uint16_t arr_index = data.index;
					
					bool pos_exists = false;
					if(data.position_exist == 1){pos_exists = true;}
					timeline_plots_position[i].timeline_settings_bool_array[arr_index] = pos_exists;				
					
					timeline_plots_position[i].timeline_points_posx[arr_index] = data.x;
					timeline_plots_position[i].timeline_points_posy[arr_index] = data.y;
					timeline_plots_position[i].timeline_points_posz[arr_index] = data.z;
					
					//add marker to timeline
					bool playback_marker_exists = false;
					if(data.playback_marker_exist == 1){playback_marker_exists = true;}
					timeline_plots_playback_markers[i].timeline_settings_bool_array[arr_index] = playback_marker_exists;
					
					timeline_plots_playback_markers[i].timeline_playback_markers[arr_index] = (PlaybackMarkerType)(data.playback_marker_type);
				}
			}
			//if system is big endian, convert little endian binary data to big endian
			else
			{
				//while have not reached end of file
				while( !infile.eof())
				{
					
					TimeFramePositionPlaybackData data;
					
					//read point from file. index, x value, y value, z value
					infile.read(reinterpret_cast <char*>( &data) ,sizeof(data));
					
					//add point to timeline
					uint16_t arr_index = ReverseInt_16bit(data.index);
					
					bool pos_exists = false;
					if(data.position_exist == 1){pos_exists = true;}
					timeline_plots_position[i].timeline_settings_bool_array[arr_index] = pos_exists;				
					
					timeline_plots_position[i].timeline_points_posx[arr_index] = ReverseFloat(data.x);
					timeline_plots_position[i].timeline_points_posy[arr_index] = ReverseFloat(data.y);
					timeline_plots_position[i].timeline_points_posz[arr_index] = ReverseFloat(data.z);
					
					//add marker to timeline
					bool playback_marker_exists = false;
					if(data.playback_marker_exist == 1){playback_marker_exists = true;}
					timeline_plots_playback_markers[i].timeline_settings_bool_array[arr_index] = playback_marker_exists;
					
					timeline_plots_playback_markers[i].timeline_playback_markers[arr_index] = (PlaybackMarkerType)(data.playback_marker_type);
				}
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
	
	//check if system is big endian
	bool systemIsBigEndian = IsSystemBigEndian();
	
	//for current selected timeline
	
	//if system is not big endian
	if(!systemIsBigEndian)
	{
		//write timeline points to file
		for(uint16_t i = 0; i < MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT; i++)
		{
			
			//if there is no timeline position point nor playback marker added
			if(!timeline_plots_position[edit_index].timeline_settings_bool_array[i] &&
				!timeline_plots_playback_markers[edit_index].timeline_settings_bool_array[i])
			{
				continue; //skip to next iteration
			}
			
			TimeFramePositionPlaybackData data;
			data.index = i;
			
			//if there is a position point added, add it to the file
			if(timeline_plots_position[edit_index].timeline_settings_bool_array[i])
			{
				data.position_exist = 1;
				
				data.x = timeline_plots_position[edit_index].timeline_points_posx[i];
				data.y = timeline_plots_position[edit_index].timeline_points_posy[i];
				data.z = timeline_plots_position[edit_index].timeline_points_posz[i];
				
			}
			else
			{
				data.position_exist = 0;
				
				data.x = 0;
				data.y = 0;
				data.z = 0;
			}
			
			//if there is a playback marker added, add it to the file
			if(timeline_plots_playback_markers[edit_index].timeline_settings_bool_array[i])
			{
				data.playback_marker_exist = 1;
				
				data.playback_marker_type = static_cast<int>(timeline_plots_playback_markers[edit_index].timeline_playback_markers[i]);
			}
			else
			{
				data.playback_marker_exist = 0;
				
				data.playback_marker_type = static_cast<int>(PlaybackMarkerType::NONE);
			}
			
			//write data
			outfile.write( reinterpret_cast <const char*>( &data) ,sizeof(data));
			
		}
	}
	//if system is big endian, convert data to little endian
	else
	{
		//write timeline points to file
		for(uint16_t i = 0; i < MAX_NUMBER_OF_POINTS_IN_TIMELINE_PLOT; i++)
		{
			
			//if there is no timeline position point nor playback marker added
			if(!timeline_plots_position[edit_index].timeline_settings_bool_array[i] &&
				!timeline_plots_playback_markers[edit_index].timeline_settings_bool_array[i])
			{
				continue; //skip to next iteration
			}
			
			TimeFramePositionPlaybackData data;
			data.index = ReverseInt_16bit(i);
			
			//if there is a position point added, add it to the file
			if(timeline_plots_position[edit_index].timeline_settings_bool_array[i])
			{
				data.position_exist = 1;
				
				data.x = ReverseFloat(timeline_plots_position[edit_index].timeline_points_posx[i]);
				data.y = ReverseFloat(timeline_plots_position[edit_index].timeline_points_posy[i]);
				data.z = ReverseFloat(timeline_plots_position[edit_index].timeline_points_posz[i]);
				
			}
			else
			{
				data.position_exist = 0;
				
				data.x = 0;
				data.y = 0;
				data.z = 0;
			}
			
			//if there is a playback marker added, add it to the file
			if(timeline_plots_playback_markers[edit_index].timeline_settings_bool_array[i])
			{
				data.playback_marker_exist = 1;
				
				data.playback_marker_type = static_cast<int>(timeline_plots_playback_markers[edit_index].timeline_playback_markers[i]);
			}
			else
			{
				data.playback_marker_exist = 0;
				
				data.playback_marker_type = static_cast<int>(PlaybackMarkerType::NONE);
			}
			
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
	
	//check if system is big endian
	bool systemIsBigEndian = IsSystemBigEndian();
	
	//if system is not big endian, assuming little endian
	if(!systemIsBigEndian)
	{
		//while have not reached end of file
		while( !infile.eof())
		{
			TimeFramePositionPlaybackData data;
			
			//read point from file. index, x value, y value, z value
			infile.read(reinterpret_cast <char*>( &data) ,sizeof(data));
			
			//add point to timeline
			uint16_t i = data.index;
			
			bool pos_exists = false;
			if(data.position_exist){pos_exists = true;}
			timeline_plots_position[edit_index].timeline_settings_bool_array[i] = pos_exists;
			
			timeline_plots_position[edit_index].timeline_points_posx[i] = data.x;
			timeline_plots_position[edit_index].timeline_points_posy[i] = data.y;
			timeline_plots_position[edit_index].timeline_points_posz[i] = data.z;
			
			bool pm_exists = false;
			if(data.playback_marker_exist){pm_exists = true;}
			
			timeline_plots_playback_markers[edit_index].timeline_settings_bool_array[i] = pm_exists;
			timeline_plots_playback_markers[edit_index].timeline_playback_markers[i] = (PlaybackMarkerType)(data.playback_marker_type);
			
		}
	}
	//else if system is big endian, convert little endian binary data to big endian
	else
	{
		//while have not reached end of file
		while( !infile.eof())
		{
			TimeFramePositionPlaybackData data;
			
			//read point from file. index, x value, y value, z value
			infile.read(reinterpret_cast <char*>( &data) ,sizeof(data));
			
			//add point to timeline
			uint16_t i = ReverseInt_16bit(data.index);
			
			bool pos_exists = false;
			if(data.position_exist){pos_exists = true;}
			timeline_plots_position[edit_index].timeline_settings_bool_array[i] = pos_exists;
			
			timeline_plots_position[edit_index].timeline_points_posx[i] = ReverseFloat(data.x);
			timeline_plots_position[edit_index].timeline_points_posy[i] = ReverseFloat(data.y);
			timeline_plots_position[edit_index].timeline_points_posz[i] = ReverseFloat(data.z);
			
			bool pm_exists = false;
			if(data.playback_marker_exist){pm_exists = true;}
			
			timeline_plots_playback_markers[edit_index].timeline_settings_bool_array[i] = pm_exists;
			timeline_plots_playback_markers[edit_index].timeline_playback_markers[i] = (PlaybackMarkerType)(data.playback_marker_type);
			
		}
	} 
		
	//close file
	infile.close();
}

void Timeline::ToggleShowTimelineParameterPropertiesBoxBool()
{
	showTimelineParameterPropertiesBox = !showTimelineParameterPropertiesBox;
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

size_t Timeline::GetCurrentTimelineFrame(){return timelineSettings.current_timeline_frame;}

void Timeline::SetCurrentTimelineFrameAtPause()
{
	timelineSettings.mouse_control = false;
	timelineSettings.frameSelected = false;	
}

void Timeline::ResetCurrentTimelineFrameToZero()
{
	timelineSettings.mouse_control = false;
	timelineSettings.frameSelected = false;
	
	timelineSettings.current_timeline_frame = 0;
}

void Timeline::InitDataDirectory(std::string filepath)
{
	m_data_dir_path = filepath;
}
