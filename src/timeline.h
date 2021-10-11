#ifndef TIMELINE_GUI_H
#define TIMELINE_GUI_H

#include "soundproducer.h"
#include "listener.h"

#include <memory>


struct TimelinePlotPosition
{
	//pointer to dynamically allocated array of bools
	float* timeline_points_posx; 
	float* timeline_points_posy; 
	float* timeline_points_posz;
	
	//bool array for timeline setting
	bool* timeline_settings_bool_array;
	
	//name of timeline plot
	std::string name;
	
	//index of the object to edit
	int indexObjectToEdit;
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
	
	void AddPlotPositionToTimeline(std::string name);
	void RemovePlotPositionFromTimeline(size_t& sound_producer_picked);
	
	void SetShowTimelineBool(bool state);
	
	//functions to tell timeline to add/remove point in current frame in timeline, if true, gets set to false after operation is finished.
	void SetAddPointToTimelineBool(bool state);
	void SetRemovePointFromTimelineBool(bool state);
	
	//function to set time frame rate, number of time frames per second
	void SetTimeFrameRate(size_t rate);
	
//loop functions
	
	//render timeline and its parameters
	void DrawGui_Item();
	
	//function to set position of listener and sound producers with timeline
	void RunPlaybackWithTimeline();
	
	//function to set timeline back to edit mode, 
	//ideally use after playback stopped or paused
	void ResumeEditModeInTimeline();

//Save / Load functions
	
	//saves timeline points to file
	void SaveTimeFramesToFile(std::string filepath);
	
	//loads timeline points from file
	void LoadTimeFramesFromFile(std::string filepath);
	
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
	
	//current timeline plot being edited
	size_t current_timeline_parameter_edited;
	
	//bools to indicate if point needs to be added or removed from timeline
	bool addPointToTimeline;
	bool removePointFromTimeline;
	
	//number of time frames to increment current frame in timeline settings after 1 second.
	size_t time_frame_rate;	
	
};

#endif
