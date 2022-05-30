#ifndef TIMELINE_GUI_H
#define TIMELINE_GUI_H

#include "soundproducer.h"
#include "listener.h"

#include "immediate_mode_sound_player.h"

#include <memory>

struct TimelinePlotPositionSaveData
{
	int edit_index;
	std::string name;
	std::string frames_filepath;
};

struct TimelineSaveData
{
	//number of timeline plot position
	size_t number_of_plots;
	
	std::vector <TimelinePlotPositionSaveData> plots_save_data;
	
};

struct TimelinePlotPosition
{
	//pointer to dynamically allocated array of floats for position
	float* timeline_points_posx; 
	float* timeline_points_posy; 
	float* timeline_points_posz;
	
	//To do: Add orientation after controls established to set orientation
	
	//bool array for timeline setting
	bool* timeline_settings_bool_array;
	
	//name of timeline plot
	std::string name;
	
	//index of the object to edit
	int indexObjectToEdit;
	
	//current filepath to frames
	std::string frames_filepath; ///overall frames file path
};

enum class PlaybackMarkerType: uint8_t {NONE=0,START_PLAY,PAUSE,RESUME,END_PLAY};

struct TimelinePlotPlaybackMarker
{
	//pointer to dyanmically allocated array of playback marker types
	PlaybackMarkerType* timeline_playback_markers;
	
	//bool array to indicate there is playback marker at this time
	bool* timeline_settings_bool_array;
	
	//index of the object to edit
	int indexObjectToEdit;
	
	
};

enum class ObjectType : uint8_t {NONE,LISTENER,SOUND_PRODUCER};

enum class AudioPlaybackSolverStatus : uint8_t {SOLVED=0,ERROR_NO_MARKERS};

class PlaybackMarkerTraveler
{
public:
	enum class TravelerState : uint8_t {NONE=0, FORWARD_TIME, PAUSED};
	
	PlaybackMarkerTraveler(){current_state = TravelerState::NONE; current_time = 0.0f;};
	
	
	void RunTravelerBasedOnPlaybackMarkerType(PlaybackMarkerType& type)
	{
		switch(type)
		{
			case PlaybackMarkerType::NONE:{ break;}
			case PlaybackMarkerType::PAUSE:{ current_state = TravelerState::PAUSED; break;}
			case PlaybackMarkerType::RESUME:{ current_state = TravelerState::FORWARD_TIME; break;}
			case PlaybackMarkerType::START_PLAY:{ current_state = TravelerState::FORWARD_TIME; break;}
			case PlaybackMarkerType::END_PLAY:{ current_state = TravelerState::NONE; current_time = 0.0f; break;}
		}
		
		switch(current_state)
		{
			case TravelerState::NONE:{ break;}
			case TravelerState::PAUSED:{ break;}
			//assuming 1 time frame is 1 second default
			case TravelerState::FORWARD_TIME:{ current_time += 1.0f; break;}
		}
	}
	
	double& GetCurrentTime(){return current_time;}
	
	TravelerState& GetCurrentState(){return current_state;}
	
	void ResetTraveler(){current_time = 0; current_state = TravelerState::NONE;}
	
private:

	TravelerState current_state;
	double current_time;
};

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
	
	//functions to add plots to timeline
	void AddPlotPositionToTimeline(std::string name);
	void RemovePlotPositionFromTimeline(size_t& index);
	
	void AddPlotPlaybackMarkerToTimeline();
	void RemovePlotPlaybackMarkerFromTimeline(size_t& index);
	
	void SetShowTimelineBool(bool state);
	
	void ToggleShowTimelineParameterPropertiesBoxBool();
	
	//functions to tell timeline to add/remove point in current frame in timeline, if true, gets set to false after operation is finished.
	void SetAddPointToTimelineBool(bool state);
	void SetRemovePointFromTimelineBool(bool state);
	
	//functions to tell timeline to add/remove playback marker in current frame in timeline.
	void SetAddPlaybackMarkerToTimelineBool(bool state, PlaybackMarkerType playback_type);
	void SetRemovePlaybackMarkerFromTimelineBool(bool state);
	
	//function to set time frame rate, number of time frames per second
	void SetTimeFrameRate(size_t rate);
	
//loop functions
	
	//render timeline and its parameters
	void DrawGui_Item();
	
	//function to solve audio playback within timeline
	void SolveAudioPlaybackInTimeline(ImmediateModeSoundPlayer* im_sound_player_ptr);
	
	//function to set position of listener and sound producers with timeline
	void RunPlaybackWithTimeline(ImmediateModeSoundPlayer* im_sound_player_ptr);
	
	//function to set timeline back to edit mode, 
	//ideally use after playback stopped or paused
	void ResumeEditModeInTimeline();
	
	void HandleInput();
	
	size_t GetCurrentTimelineFrame();
	
	void SetCurrentTimelineFrameAtPause();
	
	void ResetCurrentTimelineFrameToZero();

//Save / Load functions
	
	void LoadSaveData(TimelineSaveData& data);
	TimelineSaveData& GetSaveData();
	
	//saves timeline points to file
	void SaveTimeFramesToFile(std::string& filepath);
	
	//loads timeline points from file
	void LoadTimeFramesFromFile(std::string& filepath);
	
	void InitDataDirectory(std::string filepath);
	
	
private:
	
	//main listener
	Listener* main_listener_ptr;
	
	//pointer to vector of sound producers to manipulate
	std::vector <std::unique_ptr <SoundProducer> > *sound_producer_vector_ref;
		
	//vector of timeline identifiers for position
	std::vector <TimelinePlotPosition> timeline_plots_position;
	
	//vector of timeline identifiers for playback markers
	std::vector <TimelinePlotPlaybackMarker> timeline_plots_playback_markers;
	
	//bool to indicate to show timeline.
	bool showTimeline;
	
	//final choice for object to edit in timeline
	int m_final_edit_obj_index;
	
	//current timeline plot being edited
	size_t current_timeline_parameter_edited;
	
	//bools to indicate if point needs to be added or removed from timeline
	bool addPointToTimeline;
	bool removePointFromTimeline;
	
	bool addPlaybackMarkerToTimeline;
	PlaybackMarkerType addPlaybackMarkerToTimeline_type;
	bool removePlaybackMarkerFromTimeline;
	
	//bool to indicate if property wants to be shown
	bool showTimelineParameterPropertiesBox;
	
	//number of time frames to increment current frame in timeline settings after 1 second.
	int time_frame_rate;	
	
	//save data
	TimelineSaveData m_save_data;
	
	//helper functions to managed multiple gui draws in an organized manner.
	
	//draw points of plots
	void DrawTimelinePointsGUI();
	
	//draw timeline plot editor 
	void DrawTimelinePlotEditorGUI();
	
	//draw frames buttons and handling
	void DrawFramesGUI();
	
	//draw file dialog and handling
	void DrawFramesFileDialog();
	
	//function to show properties
	void ShowPropertiesBox(int x, int y, int timeline_index, int timeline_current_frame);
	
	//path to directory for timeline data
	std::string m_data_dir_path;
	
};

#endif
