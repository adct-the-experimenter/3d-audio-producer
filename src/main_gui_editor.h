#ifndef MAIN_GUI_EDITOR_H
#define MAIN_GUI_EDITOR_H

#include "raylib.h"

#include "openalsoftaudioengine.h"

#include "soundproducer.h"
#include "listener.h"
//#include "listener-external.h"

#include "soundproducer-registry.h"


#include "openalsoftaudioengine.h"


#include "sound_bank.h"

#include "effects-manager.h"

#include "SaveSystem.h"
#include "LoadSystem.h"


#include <iostream>
#include <memory> //for unique_ptr use

#include <sstream> //for string stream

//define thread for listener reverb

/* Define a new application type */
//the main of the application
class MainGuiEditor
{
public:
	MainGuiEditor();
	~MainGuiEditor();
	
    bool OnInit();
    
    void HandleEvents();
    
    void logic();
        
    void DrawGUI_Items();
    void Draw3DModels();
    
    Camera3D* GetPointerToCamera();
    
    void InitCamera();
    
private:
	
	std::vector < std::unique_ptr <SoundProducer> > sound_producer_vector; //vector to hold sound producers
	
	OpenAlSoftAudioEngine audio_engine; //class abstraction to handle playing binaural 3D audio
	
	//camera
	Camera3D main_camera;
	
	//listener
	std::unique_ptr <Listener> listener;
	//std::unique_ptr <ListenerExternal> listenerExternal;
	void initListener();
	
	//effects manager
	std::unique_ptr <EffectsManager> effects_manager_ptr;
	
//GUI draw and handling calls
	
	void draw_object_creation_menu();
	void draw_sound_bank();
	void draw_hrtf_menu();
	void draw_project_file_dialog();
	void draw_timeline_menu();
	
	//sound bank
	SoundBank m_sound_bank;
	
	//sound producer registry to keep track of sound producers
	SoundProducerRegistry soundproducer_registry;
	
	void CreateSoundProducer(std::string& name, float& x, float& y, float& z, bool freeRoam, std::uint8_t account_num);
	void RemoveSoundProducer(int index);
	
//Save and Load system functions
	
	//Save System
    std::unique_ptr <SaveSystem> save_system_ptr;
    
    //Load System
    std::unique_ptr <LoadSystem> load_system_ptr;
	
	//unload and free everything
	void UnloadAll();
	
	//function to create new project
	void CreateNewProject();
	
	//function to save objects and info to project file
	void SaveProject(std::string& filepath);
	
	//function to load objects and info from project file
	void LoadProject(std::string& filepath);
	
	
};

#endif // _WXSIMPLEVIEWERWX_H_
