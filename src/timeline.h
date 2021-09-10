#ifndef TIMELINE_GUI_H
#define TIMELINE_GUI_H

#include "soundproducer.h"
#include "listener.h"

#include <memory>


struct TimelinePlotPosition
{
	//pointer to dynamically allocated array
	size_t* timeline_points_posx; 
	size_t* timeline_points_posy; 
	size_t* timeline_points_posz; 
};

enum class ObjectType : uint8_t {NONE,LISTENER,SOUND_PRODUCER};

class Timeline
{
public:
	
	Timeline();
	~Timeline();
	
	void Init(std::vector <std::unique_ptr <SoundProducer> > *sound_producer_vector, Listener* listener);
	
	void InitGUI();
	
	void SetListenerInTimeline(Listener* listener_ptr);
	
	//picks object to edit in timeline from outside Timeline class
	void SetObjectPicked(int index, ObjectType type);
	
	void AddPlotPositionToTimeline();
	void RemovePlotPositionFromTimeline(size_t& sound_producer_picked);
	
	void SetShowTimelineBool(bool state);
	
//loop functions
	
	//rendering timeline
	void DrawGui_Item();
	
private:
	
	//main listener
	Listener* main_listener_ptr;
	
	//pointer to vector of sound producers to manipulate
	std::vector <std::unique_ptr <SoundProducer> > *sound_producer_vector_ref;
		
	//vector of timeline identifiers
	std::vector <TimelinePlotPosition> timeline_plots_position;
	
	//bool to indicate to show timeline.
	bool showTimeline;
	
	//final choice for object to edit in timeline
	int m_final_edit_obj_index;
};

#endif
