#ifndef TIMELINE_GUI_H
#define TIMELINE_GUI_H

#include "soundproducer.h"
#include "listener.h"


struct SoundProducerTimelinePoints
{
	SoundProducer* sound_producer_ptr;
	size_t* timeline_points; //pointer to dynamically allocated array
};


class Timeline
{
public:
	
	Timeline();
	
	void SetListenerInTimeline(Listener* listener_ptr);
	
	void AddSoundProducerToTimeline(SoundProducer* sp);
	void RemoveSoundProducerFromTimeline(size_t& sound_producer_picked);
	
	void SetShowTimelineBool(bool state);
	
//loop functions
	
	//rendering timeline
	void DrawGui_Item();
	
private:
	
	//main listener
	Listener* main_listener_ptr;
		
	//vector of sound producer timeline identifiers
	std::vector <SoundProducerTimelinePoints> timeline_sound_producers;
	
	//bool to indicate to show timeline.
	bool showTimeline;
};

#endif
