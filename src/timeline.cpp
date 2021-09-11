#include "timeline.h"

#include "raygui/raygui.h"

//define gui drop down list view
#include "raygui/gui_dropdown_listview.h"

//define timeline
#define GUI_3D_OBJECT_TIMELINE
#include "raygui/gui_timeline.h"


Timeline::Timeline()
{
	showTimeline = false;
	addPointToTimeline = false;
	removePointFromTimeline = false;
}

Timeline::~Timeline()
{
	//delete timeline plot for listener
	delete timeline_plots_position[0].timeline_points_posx;
	delete timeline_plots_position[0].timeline_points_posy;
	delete timeline_plots_position[0].timeline_points_posz;
	delete timeline_plots_position[0].timeline_settings_bool_array;
	timeline_plots_position.pop_back();
}

void Timeline::Init(std::vector <std::unique_ptr <SoundProducer> > *sound_producer_vector, Listener* listener)
{
	main_listener_ptr = listener;
	sound_producer_vector_ref = sound_producer_vector;
	
	//add first timeline plot position for listener
	Timeline::AddPlotPositionToTimeline();
}



void Timeline::SetListenerInTimeline(Listener* listener_ptr){main_listener_ptr = listener_ptr;}

void Timeline::SetAddPointToTimelineBool(bool state){addPointToTimeline = state;}

void Timeline::SetRemovePointFromTimelineBool(bool state){removePointFromTimeline = state;}

void Timeline::AddPlotPositionToTimeline()
{
	timeline_plots_position.emplace_back( TimelinePlotPosition{new float[200],new float[200],new float[200], new bool[200] });
	
	for(size_t i = 0; i < 200; i++)
	{
		timeline_plots_position.back().timeline_points_posx[i] = 0; 
		timeline_plots_position.back().timeline_points_posy[i] = 0; 
		timeline_plots_position.back().timeline_points_posz[i] = 0;

		timeline_plots_position.back().timeline_settings_bool_array[i] = false;
	}
	
}

void Timeline::RemovePlotPositionFromTimeline(size_t& sound_producer_picked)
{
	//delete points
	//increment by 1 because of listener
	delete timeline_plots_position[sound_producer_picked + 1].timeline_points_posx;
	delete timeline_plots_position[sound_producer_picked + 1].timeline_points_posy;
	delete timeline_plots_position[sound_producer_picked + 1].timeline_points_posz;
	
	std::swap(timeline_plots_position[sound_producer_picked + 1],timeline_plots_position.back());
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

static size_t max_num_frames = 200;

static TimelineSettings timelineSettings = InitTimelineSettings();

static TimelineParameterSettings positionTimelineSettings = InitTimelineParameterSettings(max_num_frames,nullptr,200,440);


//DropDownListViewSettings InitDropDownListViewSettings(int itemsShowCount, bool editMode, bool valueChanged, int scrollIndex)
static DropDownListViewSettings obj_dropdown_listview_settings = InitDropDownListViewSettings(3,false,false,0);
static int edit_obj_listview_activeIndex = 0;
static int edit_obj_listview_itemsCount = 0;


static std::string obj_choices = "";

void Timeline::InitGUI()
{
	obj_choices = "";
	
	if(sound_producer_vector_ref)
	{
		obj_choices = "Listener;";
		
		for(size_t i = 0; i < sound_producer_vector_ref->size(); i++)
		{
			obj_choices += sound_producer_vector_ref->at(i)->GetNameString() + ";";
		}
		
		edit_obj_listview_itemsCount = (int)sound_producer_vector_ref->size() + 1;
				
	}
}

void Timeline::DrawGui_Item()
{
	//draw timeline
	
	//draw drop-down box for selecting timeline of object to draw
		//listener first
		//sound producers second
	
	
	if( showTimeline )
	{
		
		//draw timeline area
		Gui_Timeline(&timelineSettings);
		
		//draw position timeline if there are points to draw
		if(positionTimelineSettings.array_points_ptr )
		{
			
			//if adding point to timeline
			if(addPointToTimeline && timelineSettings.frameSelected)
			{
				bool addPoint = false;
				float x,y,z;
				
				//if listener
				if(current_sound_producer_editing_index == -1 && m_final_edit_obj_index == 0)
				{
					//get position of listener
					x = main_listener_ptr->getPositionX();
					y = main_listener_ptr->getPositionY();
					z = main_listener_ptr->getPositionZ();
					addPoint = true;
					
				}
				//else if sound producer
				else if(current_sound_producer_editing_index != -1)
				{
					//get position of sound	producer
					x = sound_producer_vector_ref->at(current_sound_producer_editing_index)->GetPositionX();
					y = sound_producer_vector_ref->at(current_sound_producer_editing_index)->GetPositionY();
					z = sound_producer_vector_ref->at(current_sound_producer_editing_index)->GetPositionZ();
					
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
					timeline_plots_position[m_final_edit_obj_index].timeline_settings_bool_array[timelineSettings.current_timeline_frame] = true;
					
					//add point to position timeline
					timeline_plots_position[m_final_edit_obj_index].timeline_points_posx[timelineSettings.current_timeline_frame] = x; 
					timeline_plots_position[m_final_edit_obj_index].timeline_points_posy[timelineSettings.current_timeline_frame] = y; 
					timeline_plots_position[m_final_edit_obj_index].timeline_points_posz[timelineSettings.current_timeline_frame] = z;
				}
				
				addPointToTimeline = false;
			}
			else if(addPointToTimeline)
			{
				addPointToTimeline = false;
			}		
			
			if(removePointFromTimeline && timelineSettings.frameSelected)
			{
				bool removePoint = false;
				
				//if listener
				if(current_sound_producer_editing_index == -1 && m_final_edit_obj_index == 0)
				{
					removePoint = true;
					
				}
				//else if sound producer
				else if(current_sound_producer_editing_index != -1)
				{
					removePoint = true;
				}
				else
				{
					//do nothing
					removePoint = false;
				}
				
				if(removePoint)
				{
					//remove point from graphical timeline depending on frame selected
					timeline_plots_position[m_final_edit_obj_index].timeline_settings_bool_array[timelineSettings.current_timeline_frame] = false;
					
					//remoove point from position timeline with reset
					timeline_plots_position[m_final_edit_obj_index].timeline_points_posx[timelineSettings.current_timeline_frame] = 0; 
					timeline_plots_position[m_final_edit_obj_index].timeline_points_posy[timelineSettings.current_timeline_frame] = 0; 
					timeline_plots_position[m_final_edit_obj_index].timeline_points_posz[timelineSettings.current_timeline_frame] = 0;
				}
				
				removePointFromTimeline = false;
			}
			else if(removePointFromTimeline)
			{
				removePointFromTimeline = false;
			}
				
			Gui_Timeline_Parameter(&positionTimelineSettings);		
		}
		
		//draw dropdown editing box
		edit_obj_listview_activeIndex = Gui_Dropdown_ListView_Simple(&obj_dropdown_listview_settings, (Rectangle){ 100, 420, 70, 45 }, 
																obj_choices.c_str(), edit_obj_listview_itemsCount,
																edit_obj_listview_activeIndex
															);
															
		if(obj_dropdown_listview_settings.valueChanged )
		{
			m_final_edit_obj_index = edit_obj_listview_activeIndex;
			
			if(m_final_edit_obj_index != 0)
			{
				current_sound_producer_editing_index = m_final_edit_obj_index - 1;
			}
			else
			{
				current_sound_producer_editing_index = -1;
			}
			
			obj_dropdown_listview_settings.valueChanged = false;
		}
		
		
		if(edit_obj_listview_activeIndex == 0 && current_sound_producer_editing_index == -1)
		{
			//show listener position
			positionTimelineSettings.array_points_ptr = timeline_plots_position[0].timeline_settings_bool_array;
		}
		else if(edit_obj_listview_activeIndex >= 1 && current_sound_producer_editing_index != -1)
		{
			//show sound producer position
			positionTimelineSettings.array_points_ptr = timeline_plots_position[m_final_edit_obj_index].timeline_settings_bool_array;
		}
		else
		{
			positionTimelineSettings.array_points_ptr = nullptr;
		}
		
		
		
	}
	
}
