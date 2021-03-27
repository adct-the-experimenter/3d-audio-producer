#ifndef VIEWER_H
#define VIEWER_H

#include "raylib.h"

#include "openalsoftaudioengine.h"

//#include "soundproducer-track-manager.h"
#include "soundproducer.h"
#include "listener.h"
//#include "listener-external.h"

#include "soundproducer-registry.h"

#include "CreateSoundProducerDialog.h"
#include "EditMultipleSoundProducersDialog.h"
//#include "HRTF-Test-Dialog.h"
//#include "Change-HRTF-Dialog.h"
#include "EditListenerDialog.h"
//#include "setup-serial-dialog.h"

#include "openalsoftaudioengine.h"

//#include "timeline-frame.h"
//#include "soundproducer-track.h"
//#include "listener-track.h"

//#include "effects-manager.h"

//#include "SaveSystem.h"
//#include "LoadSystem.h"

#include <iostream>
#include <memory> //for unique_ptr use

#include <sstream> //for string stream


class MainFrame
{
public:
    MainFrame( const std::string& title, OpenAlSoftAudioEngine* thisAudioEngine);
		
	//for connecting mainframe to wxOsgApp
    
    void SetSoundProducerVectorRef(std::vector < std::unique_ptr <SoundProducer> > *sound_producer_vector);
    
    void SetAudioEngineReference(OpenAlSoftAudioEngine* audioEngine);
    
    void SetListenerReference(Listener* listener);
    //void SetListenerExternalReference(ListenerExternal* listenerExt);
    
    //void SetEffectsManagerReference(EffectsManager* effectsManager);
    
    //SoundProducerTrackManager* GetReferenceToSoundProducerTrackManager();
    
    // Mainframe menu operations
    
    /*
    void OnOpen(wxCommandEvent& WXUNUSED(event));
    void OnIdle(wxIdleEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	
	void OnListRightClick(wxListEvent &evt);
	void OnPopupClick(wxCommandEvent &evt);
	
	void OnNewProject(wxCommandEvent& WXUNUSED(event));
	void OnSaveProject(wxCommandEvent& WXUNUSED(event));
	void OnLoadProject(wxCommandEvent& WXUNUSED(event));
	
	void OnKeyDown(wxKeyEvent& event); //where camera viewer gets manipulated
	

	//frees all that needs to be freed before load
	void UnloadAll();
	*/
	
	friend class wxOsgApp;
	
private:
    
    enum
	{
		ID_CREATE_SOUND_PRODUCER = 1,
		ID_EDIT_MULTIPLE_SOUND_PRODUCERS,
		ID_PLAY_AUDIO,
		ID_TEST_HRTF,
		ID_CHANGE_HRTF,
		ID_LISTENER_EDIT,
		ID_SETUP_SERIAL,
		ID_CREATE_STANDARD_REVERB_ZONE,
		ID_CREATE_EAX_REVERB_ZONE,
		ID_CREATE_ECHO_ZONE,
		ID_EDIT_MULTIPLE_STANDARD_REVERB_ZONES,
		ID_EDIT_MULTIPLE_EAX_REVERB_ZONES,
		ID_EDIT_MULTIPLE_ECHO_ZONES,
		ID_NEW_PROJECT,
		ID_SAVE_PROJECT,
		ID_LOAD_PROJECT
	};
    
	OpenAlSoftAudioEngine* audioEnginePtr;
	
	std::vector < std::unique_ptr <SoundProducer> > *sound_producer_vector_ref;
	
	Listener* listenerPtr;
	//ListenerExternal* listenerExternalPtr;
	
	//ListenerTrack* m_listener_track;
	
	//TimelineFrame* timeFrame;
	
	//EffectsManager* effectsManagerPtr;
	
	//std::unique_ptr <SoundProducerTrackManager> soundproducertrack_manager_ptr;
	
	//std::vector <SoundProducerTrack*> m_soundproducer_track_vec;
	
	//wxComboBox* m_sp_toolbar_combobox;
	//wxStaticText* sp_toolbar_text;
	
	/*
	void OnCreateSoundProducer(wxCommandEvent& event); //function for menu to create and place sound producer
	void CreateSoundProducer(std::string& name, std::string& filePath, ALuint& buffer,double& x, double& y, double& z, bool freeRoam);
	void OnEditMultipleSoundProducers(wxCommandEvent& event); //function for menu to edit current available sound producers
	
	void OnTestHRTF(wxCommandEvent& event); //function for menu to test HRTF and get results
	void OnChangeHRTF(wxCommandEvent& event); //function for menu to instruct how to change HRTF with alsoft-config
    void OnEditListener(wxCommandEvent& event); //function for menu to edit listener position and orientation
    void OnSetupSerial(wxCommandEvent& event); //function for menu to setup serial communication
    
    void OnCreateStandardReverbZone(wxCommandEvent& event); //function for menu to create and place standard reverb zone
    void OnCreateEAXReverbZone(wxCommandEvent& event); //function for menu to create and place EAX reverb zone
    void OnCreateEchoZone(wxCommandEvent& event); //function for menu to create and place echo zone
    
    
    void OnEditMultipleStandardReverbZones(wxCommandEvent& event); //function for menu to edit current available reverb zones
    void OnEditMultipleEAXReverbZones(wxCommandEvent& event); //function for menu to edit current available reverb zones
    void OnEditMultipleEchoZones(wxCommandEvent& event); //function for menu to edit current available echo zones
    */
    
    SoundProducerRegistry soundproducer_registry;
    
    //wxBoxSizer* m_add_rm_box_sizer;
    
    /*
    void CreateListenerTrack();
    
    void CreateFirstSoundProducerTrack();
    
    wxButton* m_add_soundproducertrack_button;
    void OnAddSoundProducerTrack(wxCommandEvent& event);
   
    void CreateNewSoundProducerTrack();
    int sound_producer_track_count;
    wxButton* m_remove_soundproducertrack_button;
    void OnRemoveSoundProducerTrack(wxCommandEvent& event);
    
    //Save System
    std::unique_ptr <SaveSystem> save_system_ptr;
    
    //Load System
    std::unique_ptr <LoadSystem> load_system_ptr;
    
    void CreateNewProject();
    
    void SaveProject();
    
    void LoadProject();
    */
};

class wxOsgApp;

//define thread for listener reverb

/* Define a new application type */
//the main of the application
class wxOsgApp
{
public:
	wxOsgApp();
	~wxOsgApp();
	
    bool OnInit();
    
    void KeyDownLogic(int& thisKey);
    
private:
	
	
	std::vector < std::unique_ptr <SoundProducer> > sound_producer_vector; //vector to hold sound producers
	
	OpenAlSoftAudioEngine audio_engine; //class abstraction to handle playing binaural 3D audio
	
	//listener
	std::unique_ptr <Listener> listener;
	//std::unique_ptr <ListenerExternal> listenerExternal;
	void initListener();
	
	//effects manager
	//std::unique_ptr <EffectsManager> effects_manager_ptr;
	
	//define listener reverb thread here
    	
	MainFrame* frame;
};

#endif // _WXSIMPLEVIEWERWX_H_
