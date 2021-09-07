#include "timeline.h"

#include "raygui/raygui.h"

//define timeline
#define GUI_3D_OBJECT_TIMELINE
#include "raygui/raygui_extras.h"


Timeline::Timeline()
{
	showTimeline = false;
}

void Timeline::SetListenerInTimeline(Listener* listener_ptr){main_listener_ptr = listener_ptr;}

void Timeline::AddSoundProducerToTimeline(SoundProducer* sp)
{
	timeline_sound_producers.emplace_back( SoundProducerTimelinePoints{sp , new size_t[200] });
}

void Timeline::RemoveSoundProducerFromTimeline(size_t& sound_producer_picked)
{
	//delete points
	delete timeline_sound_producers[sound_producer_picked].timeline_points;
	timeline_sound_producers[sound_producer_picked].sound_producer_ptr = nullptr;
	
	std::swap(timeline_sound_producers[sound_producer_picked],timeline_sound_producers.back());
	timeline_sound_producers.pop_back();
}

void Timeline::SetShowTimelineBool(bool state){showTimeline = state;}

static size_t max_num_frames = 200;
static TimelineSettings currentTimelineSettings = InitTimelineSettings(max_num_frames,nullptr);


void Timeline::DrawGui_Item()
{
	//draw timeline
	
	if( showTimeline )
	{
		//draw drop-down box for selecting timeline of object to draw
		//listener first
		//sound producers second
		
		//draw timeline if there are points to draw
		if(currentTimelineSettings.array_points_ptr)
		{
			Gui_3dObject_Timeline(&currentTimelineSettings);
		}
		
	}
	
}
