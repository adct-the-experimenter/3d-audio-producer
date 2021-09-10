#include "timeline.h"

#include "raygui/raygui.h"

//define timeline
#define GUI_3D_OBJECT_TIMELINE
#include "raygui/raygui_extras.h"


Timeline::Timeline()
{
	showTimeline = false;
}

Timeline::~Timeline()
{
	//delete timeline plot for listener
	delete timeline_plots_position[0].timeline_points_posx;
	delete timeline_plots_position[0].timeline_points_posy;
	delete timeline_plots_position[0].timeline_points_posz;
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


void Timeline::AddPlotPositionToTimeline()
{
	timeline_plots_position.emplace_back( TimelinePlotPosition{new size_t[200],new size_t[200],new size_t[200] });
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

static size_t max_num_frames = 200;



static TimelineSettings positionXTimelineSettings = InitTimelineSettings(max_num_frames,nullptr);
static TimelineSettings positionYTimelineSettings = InitTimelineSettings(max_num_frames,nullptr);
static TimelineSettings positionZTimelineSettings = InitTimelineSettings(max_num_frames,nullptr);



//DropDownListViewSettings InitDropDownListViewSettings(int itemsShowCount, bool editMode, bool valueChanged, int scrollIndex)
static DropDownListViewSettings obj_dropdown_listview_settings = InitDropDownListViewSettings(3,false,false,0);
static int edit_obj_listview_activeIndex = 0;
static int edit_obj_listview_itemsCount = 0;

static int current_sound_producer_editing_index = -1;

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
		
		
		
		//draw position timeline if there are points to draw
		if(positionXTimelineSettings.array_points_ptr && positionYTimelineSettings.array_points_ptr 
			&& positionZTimelineSettings.array_points_ptr)
		{
			Gui_3dObject_Timeline(&positionXTimelineSettings);
			Gui_3dObject_Timeline(&positionYTimelineSettings);
			Gui_3dObject_Timeline(&positionZTimelineSettings);
		}
		
		//draw dropdown editing box
		edit_obj_listview_activeIndex = Gui_Dropdown_ListView_Simple(&obj_dropdown_listview_settings, (Rectangle){ 100, 420, 70, 45 }, 
																obj_choices.c_str(), edit_obj_listview_itemsCount,
																edit_obj_listview_activeIndex
															);
		if(obj_dropdown_listview_settings.valueChanged )
		{
			if(edit_obj_listview_activeIndex != 0)
			{
				current_sound_producer_editing_index = edit_obj_listview_activeIndex - 1;
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
			positionXTimelineSettings.array_points_ptr = timeline_plots_position[0].timeline_points_posx;
			positionYTimelineSettings.array_points_ptr = timeline_plots_position[0].timeline_points_posy;
			positionZTimelineSettings.array_points_ptr = timeline_plots_position[0].timeline_points_posz;
		}
		else if(edit_obj_listview_activeIndex >= 1 && current_sound_producer_editing_index != -1)
		{
			//show sound producer position
			positionXTimelineSettings.array_points_ptr = timeline_plots_position[current_sound_producer_editing_index].timeline_points_posx;
			positionYTimelineSettings.array_points_ptr = timeline_plots_position[current_sound_producer_editing_index].timeline_points_posy;
			positionZTimelineSettings.array_points_ptr = timeline_plots_position[current_sound_producer_editing_index].timeline_points_posz;
		}
		else
		{
			positionXTimelineSettings.array_points_ptr = nullptr;
			positionYTimelineSettings.array_points_ptr = nullptr;
			positionZTimelineSettings.array_points_ptr = nullptr;
		}
		
	}
	
}
