

#include "main_gui_editor.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui/raygui.h"

#undef RAYGUI_IMPLEMENTATION

#include "CreateSoundProducerDialog.h"
#include "EditMultipleSoundProducersDialog.h"
#include "EditListenerDialog.h"
#include "HRTF-Test-Dialog.h"
#include "Change-HRTF-Dialog.h"
#include "CreateEchoZoneDialog.h"
#include "EditMultipleEchoZonesDialog.h"
#include "CreateStandardReverbZoneDialog.h"
#include "EditMultipleStandardReverbZonesDialog.h"
#include "CreateEAXReverbZoneDialog.h"
#include "EditMultipleEAXReverbZonesDialog.h"

#include "timeline.h"

#include "immediate_mode_sound_player.h"

#define GUI_FILE_DIALOG_IMPLEMENTATION
#include "raygui/gui_file_dialog.h"


//#include "setup-serial-dialog.h"



bool init_listener_once = false;

//Gui items to initialize

static CreateSoundProducerDialog create_sp_dialog("Create Sound Producer");
static EditMultipleSoundProducersDialog edit_sp_dialog("Edit Sound Producer");
static EditListenerDialog edit_lt_dialog("Edit Listener");
static ImmediateModeSoundPlayer im_sound_player;

static HRTFTestDialog hrtf_test_dialog;
static ChangeHRTFDialog change_hrtf_dialog;

//dialogs for manipulating effect zones
static CreateEchoZoneDialog create_echo_zone_dialog;
static EditMultipleEchoZonesDialog edit_echo_zone_dialog;

static CreateStandardReverbZoneDialog create_sr_zone_dialog;
static EditMultipleStandardReverbZonesDialog edit_sr_zone_dialog;

static CreateEAXReverbZoneDialog create_er_zone_dialog;
static EditMultipleEAXReverbZonesDialog edit_er_zone_dialog;

//dialog for saving and loading project file
static GuiFileDialogState fileDialogState;
enum class ProjectFileState : std::uint8_t{NONE=0, NEW, SAVE, LOAD};
static ProjectFileState proj_file_state;

//timeline
static Timeline timeline_window;

//bool to indicate if dialog is in use.
bool global_dialog_in_use = false;

//******************************************************//
// Project file path
//******************************************************//
//bool to indicate if project is initialized, 
//used to disable functionality that requires project to be initialized
static bool project_init = false;

//string used for storing project directory path
std::string project_dir_path = "";
std::string project_file_path = "";
std::string project_data_dir_path = "";
std::string project_audio_data_dir_path = "";
std::string project_timeline_data_dir_path = "";


MainGuiEditor::MainGuiEditor()
{
	edit_sp_dialog.Init(&sound_producer_vector);
	
	
}

MainGuiEditor::~MainGuiEditor()
{
    
    //close listener reverb thread here
	effects_manager_ptr->FreeEffects();
	
}



#include <cstdio>
#include <cstdlib>
#include <bits/stdc++.h>
#include <sys/stat.h> 
#include <sys/types.h> 

// `Main program' equivalent, creating windows and returning main app frame
bool MainGuiEditor::OnInit()
{
	/*
    if (argc<2)
    {
        std::cout << wxString(argv[0]).mb_str() <<": requires filename argument." << std::endl;
        return false;
    }
	*/
	
	//create directory in home if not already made, if not windows
//#ifndef WIN32
//	system("mkdir -p $HOME/.binaural-audio-editor/resources");
//#endif

    //initialize openalsoft audio engine class
    if(!audio_engine.initOpenALSoft())
    {
		std::cout << "Fatal error. Failed to initialize OpenAL Soft! \n";
		return false;
	}
	else
	{

		MainGuiEditor::initListener();
		
		timeline_window.Init(&sound_producer_vector,listener.get());
		
		//initialize effects manager
		effects_manager_ptr = std::unique_ptr <EffectsManager>( new EffectsManager() );
		effects_manager_ptr->SetPointerToListener(listener.get());
		effects_manager_ptr->SetPointerToSoundProducerRegistry(&soundproducer_registry);
		
		create_echo_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
		
		//set pointers in immediate mode sound player
		im_sound_player.SetPointerToSoundProducerRegistry(&soundproducer_registry);
		im_sound_player.SetPointerToSoundBank(&m_sound_bank);
		im_sound_player.SetPointerToEffectsManager(effects_manager_ptr.get());
		
		//intialize sound producer registry
		soundproducer_registry.SetReferenceToSoundProducerVector(&sound_producer_vector);
		
		//initialize file dialog
		fileDialogState  = InitGuiFileDialog(GetWorkingDirectory());
		fileDialogState.windowBounds = {200,200};
		
		//initialize save system
		save_system_ptr = std::unique_ptr <SaveSystem> (new SaveSystem());
		
		//initialize load system
		load_system_ptr = std::unique_ptr <LoadSystem> (new LoadSystem());
	}


    return true;
}

void MainGuiEditor::initListener()
{
	if(!init_listener_once)
	{
		std::unique_ptr <Listener> thisListener( new Listener() );
		listener = std::move(thisListener);

		//std::cout << "\nListener initialized. Listener x:" << listener->getPositionX() << std::endl;

		if(listener.get() == nullptr){std::cout << "listener raw pointer is null in main gui editor initListener! \n";}
		else{std::cout << "\nListener raw pointer:" << listener.get() << std::endl;}

		//initialize listener external
		if(listener.get() != nullptr)
		{
			edit_lt_dialog.SetPointerToListener(listener.get());
			//std::unique_ptr <ListenerExternal> thisListenerExternal( new ListenerExternal(listener.get()) );
			//listenerExternal = std::move(thisListenerExternal);
		}

		
		init_listener_once = true;
		
	}
}


void MainGuiEditor::Draw3DModels()
{
	//draw 3d model of listener
	listener->DrawModel();
	
	//draw 3d models of sound producers
	
	if(sound_producer_vector.size() > 0)
	{
		for(size_t i = 0; i < sound_producer_vector.size(); i++)
		{
			//draw model of sound producer
			sound_producer_vector[i]->DrawModel();
		}
	}
	
	effects_manager_ptr->Draw3DModels();
	
}

float distanceToMove = 0.1f;
bool disableHotkeys = false;

bool dialogInUse = false;

// Picking line ray
Ray picker_ray = { 0 };
bool picker_ray_launched = false;

//represents the index of sound producer chosen
//signed integer used so that -1 can be used.
static std::int32_t soundproducer_picked = -1;

//represent index of effect zone picked
static int effect_zone_picked = -1;

//represent type of effect zone picked
static EffectsManager::EffectZoneType effect_zone_type_picked = EffectsManager::EffectZoneType::NONE;

//shortcut keys
static bool editKeyPressed = false;
static bool deleteKeyPressed = false;

static bool frameAddKeyPressed = false;
static bool playbackMarker_add_start_key_pressed = false;
static bool playbackMarker_add_pause_key_pressed = false;
static bool playbackMarker_add_resume_key_pressed = false;
static bool playbackMarker_add_end_key_pressed = false;

static bool showPropertiesBoxKeyPressed = false;

//listener movement variables
static float listener_speed = 10.0f;

static float listener_velocity_x = 0;
static float listener_velocity_y = 0;
static float listener_velocity_z = 0;

//sound producer movement variables
static float soundproducer_speed = 10.0f;

static float soundproducer_velocity_x = 0;
static float soundproducer_velocity_y = 0;
static float soundproducer_velocity_z = 0;

void MainGuiEditor::HandleEvents()
{
	if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		picker_ray = GetMouseRay(GetMousePosition(), main_camera);
		picker_ray_launched = true;
	}
	else{picker_ray_launched = false;}
	
	//if in sound bank area
	if(GetMouseX() > GetScreenWidth() - 200){disableHotkeys = true;}
	else{disableHotkeys = false;}
	
	//if any of these are true, do not continue to key input
	if(disableHotkeys || dialogInUse || fileDialogState.windowActive || global_dialog_in_use){return;}
	
	
	//if w key pressed
	if( IsKeyDown(KEY_W) )
	{
		listener_velocity_z = -listener_speed;
	}
	//if a key pressed
	if( IsKeyDown(KEY_A) )
	{
		listener_velocity_x = -listener_speed;
	}
	//if s key pressed
	if( IsKeyDown(KEY_S) )
	{
		listener_velocity_z = listener_speed;
	}
	//if d key pressed
	if( IsKeyDown(KEY_D) )
	{
		listener_velocity_x = listener_speed;
	}
	//if q key pressed
	if( IsKeyDown(KEY_Q) )
	{
		listener_velocity_y = -listener_speed;
	}
	//if e key pressed
	if( IsKeyDown(KEY_E) )
	{
		listener_velocity_y = listener_speed;
	}
	
	//if key 1 is pressed
	if(IsKeyReleased(KEY_ONE))
	{
		editKeyPressed = true;
	}
	else{editKeyPressed = false;}
	
	if(IsKeyReleased(KEY_DELETE))
	{
		deleteKeyPressed = true;
	}
	else{deleteKeyPressed = false;}
	
		
	//if i key pressed
	if( IsKeyDown(KEY_I) )
	{
		if(soundproducer_picked != -1)
		{	
			soundproducer_velocity_z = -soundproducer_speed;
		}
	}
	//j key pressed
	if( IsKeyDown(KEY_J) )
	{
		if(soundproducer_picked != -1)
		{	
			soundproducer_velocity_x = -soundproducer_speed;
		}
	}
	//k key pressed
	if( IsKeyDown(KEY_K) )
	{
		if(soundproducer_picked != -1)
		{	
			soundproducer_velocity_z = soundproducer_speed;
		}
	
	}
	//L key pressed
	if( IsKeyDown(KEY_L) )
	{
		if(soundproducer_picked != -1)
		{	
			soundproducer_velocity_x = soundproducer_speed;
		}
		
	}
	//u key pressed
	if( IsKeyDown(KEY_U) )
	{
		if(soundproducer_picked != -1)
		{	
			soundproducer_velocity_y = -soundproducer_speed;
		}
		
	}
	//o key pressed
	if( IsKeyDown(KEY_O) )
	{
		if(soundproducer_picked != -1)
		{	
			soundproducer_velocity_y = soundproducer_speed;
		}
		
	}
	
	//if control key down
	if( IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
	{
		//if s key down
		if(IsKeyDown(KEY_S))
		{
			proj_file_state = ProjectFileState::SAVE;
			fileDialogState.windowActive = true; //activate file dialog
		}
		//if o key down
		else if(IsKeyDown(KEY_O))
		{
			proj_file_state = ProjectFileState::LOAD;
			fileDialogState.windowActive = true; //activate file dialog
		}
	}
		
	//if b key pressed
	if( IsKeyReleased(KEY_B) )
	{
		frameAddKeyPressed = true;
	}
	
	//if z key pressed
	if( IsKeyReleased(KEY_Z) )
	{
		playbackMarker_add_start_key_pressed = true;
	}
	//if x key pressed
	if( IsKeyReleased(KEY_X) )
	{
		playbackMarker_add_pause_key_pressed = true;
	}
	//if c key pressed
	if( IsKeyReleased(KEY_C) )
	{
		playbackMarker_add_resume_key_pressed = true;
	}
	//if v key pressed
	if( IsKeyReleased(KEY_V) )
	{
		playbackMarker_add_end_key_pressed = true;
	}
	//if p key pressed
	if( IsKeyReleased(KEY_P) )
	{
		showPropertiesBoxKeyPressed = true;
	}
	
	timeline_window.HandleInput();
}

static float new_listener_position_x = 0;
static float new_listener_position_y = 0;
static float new_listener_position_z = 0;

static bool sound_player_active = false;

void MainGuiEditor::logic()
{
	float dt = GetFrameTime();
	
	//move listener
	new_listener_position_x = listener->getPositionX() + listener_velocity_x*dt;
	new_listener_position_y = listener->getPositionY() + listener_velocity_y*dt;
	new_listener_position_z = listener->getPositionZ() + listener_velocity_z*dt;
	
	listener->setPositionX(new_listener_position_x);
	listener->setPositionY(new_listener_position_y);
	listener->setPositionZ(new_listener_position_z);
	
	listener_velocity_x = 0;
	listener_velocity_y = 0;
	listener_velocity_z = 0;
	
	//move sound producer picked
	if(soundproducer_picked != -1)
	{
		float newX = sound_producer_vector.at(soundproducer_picked)->GetPositionX() + soundproducer_velocity_x*dt;
		float newY = sound_producer_vector.at(soundproducer_picked)->GetPositionY() + soundproducer_velocity_y*dt;
		float newZ = sound_producer_vector.at(soundproducer_picked)->GetPositionZ() + soundproducer_velocity_z*dt; 
		
		sound_producer_vector.at(soundproducer_picked)->SetPositionX(newX);
		sound_producer_vector.at(soundproducer_picked)->SetPositionY(newY);
		sound_producer_vector.at(soundproducer_picked)->SetPositionZ(newZ);
		
		soundproducer_velocity_x = 0;
		soundproducer_velocity_y = 0;
		soundproducer_velocity_z = 0;
	}
	
	//if picker ray launched, and dialog is not in use.
	if(picker_ray_launched && !dialogInUse)
	{
		bool collision = false;
		
		//for each soundproducer, check if picker ray hits sound producer
		if(sound_producer_vector.size() > 0)
		{
			for(size_t i = 0; i < sound_producer_vector.size(); i++)
			{
				
				float sp_x,sp_y,sp_z;
				sp_x = sound_producer_vector[i]->GetPositionX();
				sp_y = sound_producer_vector[i]->GetPositionY();
				sp_z = sound_producer_vector[i]->GetPositionZ();
				
				float cube_width = 2.0f;
				
				// Check collision between ray and box
				collision = GetRayCollisionBox( picker_ray,
						(BoundingBox){
							(Vector3){ sp_x - cube_width/2, sp_y - cube_width/2, sp_z - cube_width/2 },
							(Vector3){ sp_x + cube_width/2, sp_y + cube_width/2, sp_z + cube_width/2 }}
						).hit;
				
				sound_producer_vector[i]->SetPickedBool(collision);
				
				//break loop if one collision has happened
				if(collision)
				{
					soundproducer_picked = i; 
					
					break;
				}
			}
				
		}
		
		if(!collision)
		{
			soundproducer_picked = -1;
			
			//if sound producer is not picked,
			//check for collison with effects zone.
			int index = -1;
			EffectsManager::EffectZoneType type = EffectsManager::EffectZoneType::NONE;
			
			effects_manager_ptr->CheckEffectZones3DPicking(picker_ray, type , index );
			
			if(index != -1 && type != EffectsManager::EffectZoneType::NONE)
			{
				effect_zone_type_picked = type;
				effect_zone_picked = index;
				effects_manager_ptr->SetEffectZonePicked(true,effect_zone_type_picked, effect_zone_picked);
			}
			else
			{
				effects_manager_ptr->SetEffectZonePicked(false,effect_zone_type_picked, effect_zone_picked);
				effect_zone_type_picked = type;
				effect_zone_picked = index;
			}
		}
		
	}
	
	if(deleteKeyPressed && !sound_player_active)
	{
		if(soundproducer_picked != -1)
		{
			//std::cout << "delete sound producer " << soundproducer_picked << std::endl;
			
			MainGuiEditor::RemoveSoundProducer(soundproducer_picked);
			
			soundproducer_picked = -1;
		}
		
		if(effect_zone_picked != -1)
		{
			switch(effect_zone_type_picked)
			{
				case EffectsManager::EffectZoneType::STANDARD_REVERB:{ effects_manager_ptr->RemoveStandardReverbZone(effect_zone_picked); break;}
				case EffectsManager::EffectZoneType::EAX_REVERB:{ effects_manager_ptr->RemoveEAXReverbZone(effect_zone_picked); break;}
				case EffectsManager::EffectZoneType::ECHO:{ effects_manager_ptr->RemoveEchoZone(effect_zone_picked); break;}
				default:{ break;}
			}
			
			effect_zone_picked = -1;
			effect_zone_type_picked = EffectsManager::EffectZoneType::NONE;
		}
	}
	
	//run state for immediate mode sound player
	im_sound_player.RunStateForPlayer_ComplexPlayback();
	
	sound_player_active = im_sound_player.PlayerInActiveUse();
		
}

void MainGuiEditor::DrawGUI_Items()
{
	//draw project file buttons
	MainGuiEditor::draw_project_file_dialog();
	
	if(!project_init){return;} //skip if project is not initialized
	
	//draw sound bank
	MainGuiEditor::draw_sound_bank();
	
	//draw HRTF edit menu
	MainGuiEditor::draw_hrtf_menu();
	
	//draw object creation/edit menu
	MainGuiEditor::draw_object_creation_menu();
	
	//draw timeline 
	MainGuiEditor::draw_timeline_menu();
}

bool dropDownObjectTypeMode = false;
int dropDownObjectTypeActive = 0;
bool objectManipulationState = false;


//gui state
enum class OurGuiState : std::uint8_t { NONE=0,
									CREATE_SOUND_PRODUCER, EDIT_SOUND_PRODUCER, 
									EDIT_LISTENER, 
									TEST_HRTF, CHANGE_HRTF, 
									CREATE_ECHO_ZONE, EDIT_ECHO_ZONE,
									CREATE_SR_ZONE, EDIT_SR_ZONE,
									CREATE_ER_ZONE, EDIT_ER_ZONE };
									
static OurGuiState g_state = OurGuiState::NONE;

void MainGuiEditor::draw_object_creation_menu()
{
	
	if(objectManipulationState){}
	
	//create,edit object menu panel
	//draw rectangle panel on the left
	
	GuiDrawRectangle((Rectangle){20,120,150,160}, 1, BLACK, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)) );
	//GuiPanel( (Rectangle){20,80,125,160} );
	GuiDrawText("Object Creation / Edit", (Rectangle){20,120,125,20}, 1, BLACK);
	
	//draw button create
	bool createObjectClicked = GuiButton( (Rectangle){ 25, 180, 70, 30 }, "Create" );
	//draw button edit
	bool editObjectClicked = GuiButton( (Rectangle){ 25, 220, 70, 30 }, "Edit" );
	
	//draw GuiDropdownBox for choosing type to manipulate
	
	if( GuiDropdownBox((Rectangle){ 25,140,140,30 }, "None;Listener;Sound Producer;Standard Reverb Zone; EAX Reverb Zone; Echo Zone", &dropDownObjectTypeActive, dropDownObjectTypeMode) )
	{
		dropDownObjectTypeMode = !dropDownObjectTypeMode;
	}
	
	//if object to manipulate is not none, and not in a state of object manipulation
	if(dropDownObjectTypeActive != 0 && !objectManipulationState)
	{
		//if create object button clicked on
		if(createObjectClicked)
		{
			create_sp_dialog.SetPointerToSoundBank(&m_sound_bank);
			create_sp_dialog.InitSoundBankChoices();
			
			switch(dropDownObjectTypeActive)
			{
				//sound producer
				case 2:{ g_state = OurGuiState::CREATE_SOUND_PRODUCER; dialogInUse = true; break;}
				//standard reverb zone
				case 3:{ g_state = OurGuiState::CREATE_SR_ZONE; dialogInUse = true; break;}
				//eax reverb zone
				case 4:{ g_state = OurGuiState::CREATE_ER_ZONE; dialogInUse = true; break;}
				//echo zone
				case 5:{ g_state = OurGuiState::CREATE_ECHO_ZONE; dialogInUse = true; break;}
				default:{break;}
			}
		}
		
		//if edit object button clicked on
		if(editObjectClicked)
		{
			
			switch(dropDownObjectTypeActive)
			{
				//listener
				case 1:
				{ 
					g_state = OurGuiState::EDIT_LISTENER;
					dialogInUse = true;
					edit_lt_dialog.InitGUI();
					break;
				}
				//sound producer
				case 2:
				{ 
					g_state = OurGuiState::EDIT_SOUND_PRODUCER;
					dialogInUse = true;
					edit_sp_dialog.SetPointerToSoundBank(&m_sound_bank);
					edit_sp_dialog.InitGUI();
					break;
				}
				//standard reverb zone
				case 3:
				{ 
					g_state = OurGuiState::EDIT_SR_ZONE;
					dialogInUse = true;
					edit_sr_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
					edit_sr_zone_dialog.InitGUI();
					break;
				}
				//eax reverb zone
				case 4:
				{
					g_state = OurGuiState::EDIT_ER_ZONE;
					dialogInUse = true;
					edit_er_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
					edit_er_zone_dialog.InitGUI();
					break;
				}
				//echo zone
				case 5:
				{ 
					g_state = OurGuiState::EDIT_ECHO_ZONE;
					dialogInUse = true;
					edit_echo_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
					edit_echo_zone_dialog.InitGUI(); 
					break;
				}
				default:{break;}
			}
		}
	}
	
	if(soundproducer_picked != -1 && editKeyPressed)
	{
		editKeyPressed = false;
		g_state = OurGuiState::EDIT_SOUND_PRODUCER;
		dialogInUse = true;
		edit_sp_dialog.SetCurrentSoundProducerEditedIndex(size_t(soundproducer_picked));
		edit_sp_dialog.SetPointerToSoundBank(&m_sound_bank);
		edit_sp_dialog.InitGUI();
	}
	
	//if effect zone index is not null, effect type not null, and edit key pressed
	if(effect_zone_picked != -1 && effect_zone_type_picked != EffectsManager::EffectZoneType::NONE
	   && editKeyPressed)
	{
		
		effects_manager_ptr->SetEffectZonePicked(false,effect_zone_type_picked, effect_zone_picked);
		
		switch(effect_zone_type_picked)
		{
			case EffectsManager::EffectZoneType::STANDARD_REVERB:
			{ 
				g_state = OurGuiState::EDIT_SR_ZONE;
				edit_sr_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
				edit_sr_zone_dialog.SetCurrentZoneIndexForEditing(size_t(effect_zone_picked));
				edit_sr_zone_dialog.InitGUI(); 
				break;
			}
			case EffectsManager::EffectZoneType::EAX_REVERB:
			{
				g_state = OurGuiState::EDIT_ER_ZONE;
				edit_er_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
				edit_er_zone_dialog.SetCurrentZoneIndexForEditing(size_t(effect_zone_picked));
				edit_er_zone_dialog.InitGUI();
				break;
			}
			case EffectsManager::EffectZoneType::ECHO:
			{
				g_state = OurGuiState::EDIT_ECHO_ZONE;
				edit_echo_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
				edit_echo_zone_dialog.SetCurrentZoneIndexForEditing(size_t(effect_zone_picked));
				edit_echo_zone_dialog.InitGUI(); 
				break;
			}
			default:{break;}
		}
	}
	
	switch(g_state)
	{
		case OurGuiState::EDIT_LISTENER:
		{
			edit_lt_dialog.DrawDialog();
			
			if(edit_lt_dialog.OkClickedOn() || edit_lt_dialog.CancelClickedOn())
			{
				g_state = OurGuiState::NONE;
				create_sp_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case OurGuiState::CREATE_SOUND_PRODUCER:
		{
			create_sp_dialog.DrawDialog();
			
			if(create_sp_dialog.OkClickedOn())
			{
				//create sound producer
				std::string name = create_sp_dialog.getNewName();
				float x,y,z;
				create_sp_dialog.getNewPosition(x,y,z);
				bool freeRoam = create_sp_dialog.getFreeRoamBool();
				std::uint8_t account_num =  create_sp_dialog.getAccountNumber();
				MainGuiEditor::CreateSoundProducer(name,x, y, z, freeRoam, account_num);
									
				g_state = OurGuiState::NONE;
				create_sp_dialog.resetConfig();
				dialogInUse = false;
			}
			
			if(create_sp_dialog.CancelClickedOn())
			{
				g_state = OurGuiState::NONE;
				create_sp_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case OurGuiState::EDIT_SOUND_PRODUCER:
		{
			edit_sp_dialog.DrawDialog();
			
			if(edit_sp_dialog.OkClickedOn() || edit_sp_dialog.CancelClickedOn())
			{						
				g_state = OurGuiState::NONE;
				edit_sp_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case OurGuiState::CREATE_ECHO_ZONE:
		{
			create_echo_zone_dialog.DrawDialog();
			
			if(create_echo_zone_dialog.OkClickedOn() )
			{
				//create echo zone
				float x,y,z,width;
				EchoZoneProperties properties;
				
				create_echo_zone_dialog.getNewPosition(x,y,z);
				std::string name = create_echo_zone_dialog.getNewName();
				width = create_echo_zone_dialog.getNewWidth();
				properties = create_echo_zone_dialog.getNewProperties();
				
				effects_manager_ptr->CreateEchoZone(name,x,y,z,width,properties);
				
				g_state = OurGuiState::NONE;
				create_echo_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			if(create_echo_zone_dialog.CancelClickedOn())
			{
				g_state = OurGuiState::NONE;
				create_echo_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case OurGuiState::EDIT_ECHO_ZONE:
		{
			edit_echo_zone_dialog.DrawDialog();
			
			if(edit_echo_zone_dialog.OkClickedOn() || edit_echo_zone_dialog.CancelClickedOn())
			{						
				g_state = OurGuiState::NONE;
				edit_echo_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case OurGuiState::CREATE_SR_ZONE:
		{
			create_sr_zone_dialog.DrawDialog();
			
			if(create_sr_zone_dialog.OkClickedOn() )
			{
				//create standard reverb zone
				float x,y,z,width;
				ReverbStandardProperties properties;
				
				create_sr_zone_dialog.getNewPosition(x,y,z);
				std::string name = create_sr_zone_dialog.getNewName();
				width = create_sr_zone_dialog.getNewWidth();
				properties = create_sr_zone_dialog.getNewProperties();
				
				effects_manager_ptr->CreateStandardReverbZone(name,x,y,z,width,properties);
				
				g_state = OurGuiState::NONE;
				create_sr_zone_dialog.resetConfig();
				dialogInUse = false;
			}

			if(create_sr_zone_dialog.CancelClickedOn())
			{
				g_state = OurGuiState::NONE;
				create_sr_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case OurGuiState::EDIT_SR_ZONE:
		{
			edit_sr_zone_dialog.DrawDialog();
			
			if(edit_sr_zone_dialog.OkClickedOn() || edit_sr_zone_dialog.CancelClickedOn())
			{						
				g_state = OurGuiState::NONE;
				edit_sr_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			break;
		}
		case OurGuiState::CREATE_ER_ZONE:
		{
			create_er_zone_dialog.DrawDialog();
			
			if(create_er_zone_dialog.OkClickedOn()  )
			{
				//create eax reverb zone
				float x,y,z,width;
				ReverbEAXProperties properties;
				
				create_er_zone_dialog.getNewPosition(x,y,z);
				std::string name = create_er_zone_dialog.getNewName();
				width = create_er_zone_dialog.getNewWidth();
				properties = create_er_zone_dialog.getNewProperties();
				
				effects_manager_ptr->CreateEAXReverbZone(name,x,y,z,width,properties);
				
				g_state = OurGuiState::NONE;
				create_sr_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			if(create_er_zone_dialog.CancelClickedOn())
			{
				g_state = OurGuiState::NONE;
				create_er_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case OurGuiState::EDIT_ER_ZONE:
		{
			edit_er_zone_dialog.DrawDialog();
			
			if(edit_er_zone_dialog.OkClickedOn() || edit_er_zone_dialog.CancelClickedOn())
			{						
				g_state = OurGuiState::NONE;
				edit_er_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		default:{ break;}
	}
}


void MainGuiEditor::draw_sound_bank()
{
	//draw on right side of screen
	
	float leftX = GetScreenWidth() - 200;
	
	GuiDrawRectangle((Rectangle){leftX,50,200,350}, 1, BLACK, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)) );
	GuiDrawText("Sound Bank", (Rectangle){leftX,50,125,20}, 1, BLACK);
	GuiDrawText("Sound Name", (Rectangle){leftX,70,125,20}, 1, BLACK);
	GuiDrawText("File", (Rectangle){leftX + 75,70,125,20}, 1, BLACK);
	
	m_sound_bank.DrawGui_Item();
	
}

static bool show_timeline_toggle_bool = false;


void MainGuiEditor::draw_timeline_menu()
{
	//draw timeline playback controls
	float center_x = GetScreenWidth() / 2;
	
	//draw play button
	if(GuiButton( (Rectangle){ center_x - 100, 50, 50, 30 }, "Play" ))
	{
		//initialize audio player
		im_sound_player.InitPlayer_ComplexPlayback();
		
		//solve audio playback in timeline according to timeline playback marker plot
		timeline_window.SolveAudioPlaybackInTimeline(&im_sound_player);
		
		//calculate current time from current timeline frame
		//assuming 1 second per timeline frame, and ignore timeline frame rate for now		
		double time = timeline_window.GetCurrentTimelineFrame();
				
		//set time in sound player.
		im_sound_player.SetCurrentTimeInSoundPlayer(time);
		
		im_sound_player.StartPlayback_ComplexPlayback();
		
		
	}
	
	//draw pause button
	if(GuiButton( (Rectangle){ center_x, 50, 50, 30 },  "Pause" ))
	{
		im_sound_player.PausePlayback_ComplexPlayback();
		timeline_window.SetCurrentTimelineFrameAtPause();
	}
	
	//draw stop button
	if(GuiButton( (Rectangle){ center_x + 100, 50, 50, 30 }, "Stop"))
	{
		im_sound_player.StopPlayback_ComplexPlayback();
		timeline_window.ResetCurrentTimelineFrameToZero();
	}
	
	//if need to add point or playback marker to timeline
	
	//draw on bottom third of screen	
	timeline_window.DrawGui_Item();
	
	//draw show timeline button
	if( GuiButton( (Rectangle){ 25, 420, 70, 30 }, "Timeline" ) )
	{
		show_timeline_toggle_bool = !show_timeline_toggle_bool;
		
		timeline_window.SetShowTimelineBool(show_timeline_toggle_bool);
	}
	
	//if timeline is shown
	if(show_timeline_toggle_bool)
	{
		timeline_window.InitGUI();
		
		if(frameAddKeyPressed)
		{
			timeline_window.SetAddPointToTimelineBool(true);
			frameAddKeyPressed = false;
		}
		else if(deleteKeyPressed)
		{
			timeline_window.SetRemovePointFromTimelineBool(true);
			timeline_window.SetRemovePlaybackMarkerFromTimelineBool(true);
			deleteKeyPressed = false;
		}
		else if(playbackMarker_add_start_key_pressed)
		{
			timeline_window.SetAddPlaybackMarkerToTimelineBool(true,PlaybackMarkerType::START_PLAY);
			playbackMarker_add_start_key_pressed = false;
		}
		else if(playbackMarker_add_pause_key_pressed)
		{
			timeline_window.SetAddPlaybackMarkerToTimelineBool(true,PlaybackMarkerType::PAUSE);
			playbackMarker_add_pause_key_pressed = false;
		}
		else if(playbackMarker_add_resume_key_pressed)
		{
			timeline_window.SetAddPlaybackMarkerToTimelineBool(true,PlaybackMarkerType::RESUME);
			playbackMarker_add_resume_key_pressed = false;
		}
		else if(playbackMarker_add_end_key_pressed)
		{
			timeline_window.SetAddPlaybackMarkerToTimelineBool(true,PlaybackMarkerType::END_PLAY);
			playbackMarker_add_end_key_pressed = false;
		}
		else if(showPropertiesBoxKeyPressed)
		{
			timeline_window.ToggleShowTimelineParameterPropertiesBoxBool();
			showPropertiesBoxKeyPressed = false;
		}
		
	}
	
	//if sound player is in active use
	if(im_sound_player.PlayerInActiveUse())
	{
		timeline_window.RunPlaybackWithTimeline(&im_sound_player);
	}
	else
	{
		timeline_window.ResumeEditModeInTimeline();
	}
	
	
}

void MainGuiEditor::draw_hrtf_menu()
{
	GuiDrawRectangle((Rectangle){20,300,150,100}, 1, BLACK, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)) );
	GuiDrawText("HRTF", (Rectangle){20,300,125,20}, 1, BLACK);
	
	//draw button Test
	bool testHRTFButtonClicked = GuiButton( (Rectangle){ 25, 320, 70, 30 }, "Test" );
	//draw button Change
	bool changeHRTFButtonClicked = GuiButton( (Rectangle){ 25, 360, 70, 30 }, "Change" );
	
	if(testHRTFButtonClicked)
	{
		g_state = OurGuiState::TEST_HRTF;
		dialogInUse = true;
		hrtf_test_dialog.SetPointerToAudioEngine(&audio_engine);
		hrtf_test_dialog.InitGUI();
	}
	
	if(changeHRTFButtonClicked)
	{
		g_state = OurGuiState::CHANGE_HRTF;
		dialogInUse = true;
		change_hrtf_dialog.SetPointerToAudioEngine(&audio_engine);
		change_hrtf_dialog.InitGUI();
	}
	
	switch(g_state)
	{
		case OurGuiState::TEST_HRTF:
		{
			hrtf_test_dialog.DrawDialog();
			
			if(hrtf_test_dialog.OkClickedOn())
			{
				g_state = OurGuiState::NONE;
				dialogInUse = false;
				hrtf_test_dialog.resetConfig();
			}
			break;
		}
		case OurGuiState::CHANGE_HRTF:
		{
			change_hrtf_dialog.DrawDialog();
			
			if(change_hrtf_dialog.OkClickedOn())
			{
				g_state = OurGuiState::NONE;
				dialogInUse = false;
				change_hrtf_dialog.resetConfig();
			}
			break;
		}
		default:{break;}
	}
}

void MainGuiEditor::draw_project_file_dialog()
{
	GuiDrawRectangle((Rectangle){20,0,150,100}, 1, BLACK, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)) );
	GuiDrawText("Project", (Rectangle){20,10,125,20}, 1, BLACK);
	
	//draw new project button
	if( GuiButton( (Rectangle){ 25, 0, 90, 30 }, GuiIconText(ICON_FILE_OPEN, "New Project") ) )
	{
		proj_file_state = ProjectFileState::NEW;
		fileDialogState.windowActive = true; //activate file dialog
	}
	
	//draw load project button
	if( GuiButton( (Rectangle){ 25, 35, 90, 30 }, GuiIconText(ICON_FILE_OPEN, "Load Project") ) )
	{
		proj_file_state = ProjectFileState::LOAD;
		fileDialogState.windowActive = true; //activate file dialog
	}
	
	//draw save project button if project initialized
	if( project_init && GuiButton( (Rectangle){ 25, 70, 90, 30 }, GuiIconText(ICON_FILE_SAVE, "Save Project") ) )
	{
		proj_file_state = ProjectFileState::SAVE;
		fileDialogState.windowActive = false; //do not activate file dialog
		MainGuiEditor::SaveProject(project_file_path);
		proj_file_state = ProjectFileState::NONE;
	}
	
	
	if (fileDialogState.windowActive){ GuiLock();}
	
	if(proj_file_state == ProjectFileState::NEW)
	{
		if (fileDialogState.SelectFilePressed && fileDialogState.itemFocused == -1)
		{
			char projectName[512] = { 0 };
			char projectDirPath[512] = { 0 };
			
			std::string name = std::string(fileDialogState.fileNameTextBoxInputCopy);
			std::string top_dir = std::string(fileDialogState.dirPathText);
						
			//make new project
			MainGuiEditor::CreateNewProject(name,top_dir);
			
			
		}
		
		fileDialogState.SelectFilePressed = false;
	}
	else if(proj_file_state == ProjectFileState::LOAD)
	{
		if (fileDialogState.SelectFilePressed)
		{			
			// Load project file (if supported extension)
			if (IsFileExtension(fileDialogState.fileNameText, ".xml") )
			{
				char projectFile[512] = { 0 };
				
				strcpy(projectFile, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
				std::string filepath = std::string(projectFile);
				
				std::cout << "load filepath in main gui editor: " << filepath << std::endl;
				
				//load project
				MainGuiEditor::LoadProject(filepath);
			}

			fileDialogState.SelectFilePressed = false;
		}
		
	}
	else if(proj_file_state == ProjectFileState::SAVE)
	{
		//do nothing, no file dialog operation needed
	}
	
	GuiUnlock();
	
	// call GuiFileDialog menu
	GuiFileDialog(&fileDialogState);
}

void MainGuiEditor::InitCamera()
{
	main_camera = { 0 };
    main_camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    main_camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    main_camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    main_camera.fovy = 45.0f;                                // Camera field-of-view Y
    main_camera.type = CAMERA_PERSPECTIVE;                   // Camera mode type
    
    SetCameraMode(main_camera, CAMERA_FREE);
}

Camera3D* MainGuiEditor::GetPointerToCamera()
{
	return &main_camera;
}

void MainGuiEditor::UnloadAll()
{
	//free effects
	effects_manager_ptr->RemoveEffectFromAllSources();
	effects_manager_ptr->FreeEffects();
	
	//clear sound producer registry
	soundproducer_registry.ClearAll();
	
	//remove all sound producers in vector
	RemoveAllSoundProducersSafely();
	
	//reset sound player 
	im_sound_player.ResetPlayers_ComplexPlayback();
}

void MainGuiEditor::CreateNewProject(std::string& project_name, std::string& proj_top_dir)
{
	project_init = false;
	
	//free everything
	MainGuiEditor::UnloadAll();
	
	//make new project directory
	#if defined(WIN32)
	project_dir_path = proj_top_dir + "\\" + project_name;
	if( mkdir( project_dir_path.c_str() ) != 0 )
	{
		std::cout << "Failed to create project directory!\n";
		return;
	}
	#else
	project_dir_path = proj_top_dir + "/" + project_name;
	if(mkdir(project_dir_path.c_str(),0777) != 0)
	{
		std::cout << "Failed to create project directory!\n";
		return;
	}
	#endif
	
	//make new data directory in project directory
	#if defined(WIN32)
	project_data_dir_path = project_dir_path + "\\" + "data\\";
	if( mkdir( project_data_dir_path.c_str() ) != 0 )
	{
		std::cout << "Failed to create project data directory!\n";
		return;
	}
	#else
	project_data_dir_path = project_dir_path + "/" + "data/";
	if(mkdir(project_data_dir_path.c_str(),0777) != 0)
	{
		std::cout << "Failed to create project data directory!\n";
		return;
	}
	#endif
	
	//make new audio data directory in project data directory
	#if defined(WIN32)
	project_audio_data_dir_path = project_dir_path + "\\" + "data\\" + "audio\\";
	if( mkdir( project_audio_data_dir_path.c_str() ) != 0 )
	{
		std::cout << "Failed to create project audio data directory!\n";
		return;
	}
	#else
	project_audio_data_dir_path = project_dir_path + "/" + "data/" + "audio/";
	if(mkdir(project_audio_data_dir_path.c_str(),0777) != 0)
	{
		std::cout << "Failed to create project audio data directory!\n";
		return;
	}
	#endif
	
	//make new timeline data directory in project data directory
	#if defined(WIN32)
	project_timeline_data_dir_path = project_dir_path + "\\" + "data\\" + "timeline\\";
	if( mkdir( project_timeline_data_dir_path.c_str() ) != 0 )
	{
		std::cout << "Failed to create project timeline data directory!\n";
		return;
	}
	#else
	project_timeline_data_dir_path = project_dir_path + "/" + "data/" + "timeline/";
	if(mkdir(project_timeline_data_dir_path.c_str(),0777) != 0)
	{
		std::cout << "Failed to create project timeline data directory!\n";
		return;
	}
	#endif
	
	//save new project file
	
	#if defined(WIN32)
	project_file_path = project_dir_path + "\\" + project_name + ".xml";
	#else
	project_file_path = project_dir_path + "/" + project_name + ".xml";
	#endif
	
	
	MainGuiEditor::SaveProject(project_file_path);
	
	//initialize sound bank with new data directory
	m_sound_bank.InitDataDirectory(project_audio_data_dir_path);
	
	//initialize timeline window with new data directory
	timeline_window.InitDataDirectory(project_timeline_data_dir_path);
	
	project_init = true;
}

void MainGuiEditor::SaveProject(std::string& filepath)
{
	std::string saveFilePath = filepath;
	
	//if .xml is not in ending of file name
	if(saveFilePath.substr(saveFilePath.length() - 4,saveFilePath.length() - 1) != ".xml")
	{
		saveFilePath.append(".xml");
	}
	
	std::cout << "Save project file path:" << saveFilePath << std::endl;
	
	save_system_ptr->SetSaveFilePath(saveFilePath);
	
	SaveSystemDataHelper save_system_data_helper; 
	save_system_data_helper.sound_producer_vector_ptr = &sound_producer_vector;
	save_system_data_helper.effectsManagerPtr = effects_manager_ptr.get();
	save_system_data_helper.listener_ptr = listener.get();
	save_system_data_helper.sound_bank_ptr = &m_sound_bank;
	save_system_data_helper.timeline_ptr = &timeline_window;
	
	save_system_ptr->SaveProjectToSetFile(save_system_data_helper);
	
}

//function to extract directory from project file path
static bool ExtractDirectoryFromProjectFilepath(std::string& filepath, std::string& dir_filepath)
{
	
	//for non-windows, assuming unix and unix like, find last occurence of / in filepath
	//for windows, find last occurence of '\\' 
	std::size_t pos_found = 0;
	
	pos_found = filepath.find_last_of("/\\");
	
	if( pos_found == 0)
	{
		std::cout << "Unable to find any slashes in filepath!\n";
		return false;
	}
	
	//erase anything after the last occurence of a slash
	dir_filepath = filepath.substr(0,pos_found + 1 );
	
	return true;
}

void MainGuiEditor::LoadProject(std::string& filepath)
{
	
	//Start new project
	
	//free everything
	MainGuiEditor::UnloadAll();
	
	project_init = false;
	
	//set objects in new projects based on saved data read
			
	std::cout << "Input project load file path:" << filepath << std::endl;
	
	
	if(!ExtractDirectoryFromProjectFilepath(filepath,project_dir_path))
	{
		std::cout << "\n\nFailed to extract directory from project file path!\n";
	}
	
	std::cout << "\nDirectory file path: " << project_dir_path << std::endl;
	
	project_file_path = filepath;
	
	#if defined(WIN32)
	project_data_dir_path = project_dir_path + "data\\";
	project_audio_data_dir_path = project_data_dir_path + "audio\\";
	project_timeline_data_dir_path = project_data_dir_path + "timeline\\";
	#else
	project_data_dir_path = project_dir_path + "data/";
	project_audio_data_dir_path = project_data_dir_path + "audio/";
	project_timeline_data_dir_path = project_data_dir_path + "timeline/";
	#endif
	
	std::vector <SoundProducerSaveData> sound_producer_save_data;

	std::vector <EchoZoneSaveData> echoZonesSaveData;
	std::vector <StandardReverbZoneSaveData> standardRevZonesSaveData;
	std::vector <EAXReverbZoneSaveData> eaxRevZonesSaveData;
	
	SoundBankSaveData sound_bank_save_data;

	ListenerSaveData listener_data;
	
	TimelineSaveData timeline_save_data;
	
	LoadDataHelper load_data_helper;
	load_data_helper.sound_producer_save_data = &sound_producer_save_data;
	load_data_helper.echoZonesSaveData = &echoZonesSaveData;
	load_data_helper.standardRevZonesSaveData = &standardRevZonesSaveData;
	load_data_helper.eaxRevZonesSaveData = &eaxRevZonesSaveData;
	load_data_helper.listener_data_ptr = &listener_data;
	load_data_helper.sound_bank_save_data_ptr = &sound_bank_save_data;
	
	load_data_helper.timeline_save_data_ptr = &timeline_save_data;
	
	load_system_ptr->LoadProject(load_data_helper,filepath);
						   
	
	//initialize sound producers from save data
	if(sound_producer_save_data.size() > 0)
	{
		for(size_t i = 0; i < sound_producer_save_data.size(); i++)
		{
			//create sound producer
			MainGuiEditor::CreateSoundProducer(sound_producer_save_data[i].name,
												sound_producer_save_data[i].x, sound_producer_save_data[i].y, sound_producer_save_data[i].z, 
												sound_producer_save_data[i].freeRoam, sound_producer_save_data[i].account_number);
		}
	}
	
	if(echoZonesSaveData.size() > 0)
	{
		for(size_t i = 0; i < echoZonesSaveData.size(); i++)
		{
			
			effects_manager_ptr->CreateEchoZone(echoZonesSaveData[i].name,
											 echoZonesSaveData[i].x,
											 echoZonesSaveData[i].y,
											 echoZonesSaveData[i].z,
											 echoZonesSaveData[i].width,
											 echoZonesSaveData[i].properties);
	
		}
	}
	
	if(standardRevZonesSaveData.size() > 0)
	{
		for(size_t i = 0; i < standardRevZonesSaveData.size(); i++)
		{
			
			effects_manager_ptr->CreateStandardReverbZone(standardRevZonesSaveData[i].name,
											 standardRevZonesSaveData[i].x,
											 standardRevZonesSaveData[i].y,
											 standardRevZonesSaveData[i].z,
											 standardRevZonesSaveData[i].width,
											 standardRevZonesSaveData[i].properties);
	
		}
	}
	
	if(eaxRevZonesSaveData.size() > 0)
	{
		for(size_t i = 0; i < eaxRevZonesSaveData.size(); i++)
		{
			
			effects_manager_ptr->CreateEAXReverbZone(eaxRevZonesSaveData[i].name,
											 eaxRevZonesSaveData[i].x,
											 eaxRevZonesSaveData[i].y,
											 eaxRevZonesSaveData[i].z,
											 eaxRevZonesSaveData[i].width,
											 eaxRevZonesSaveData[i].properties);
	
		}
	}
	
	//initialize listener from save data
	if(listener)
	{
		listener->LoadListenerSaveData(listener_data);
	}
	
	//initialize sound bank from save data and data directory
	m_sound_bank.InitDataDirectory(project_audio_data_dir_path);
	m_sound_bank.LoadSaveData(sound_bank_save_data);
	
	//initialize timeline from save data and data directory
	timeline_window.InitDataDirectory(project_timeline_data_dir_path);
	timeline_window.LoadSaveData(timeline_save_data);
	
	project_init = true;
	
}


void MainGuiEditor::CreateSoundProducer(std::string& name,float& x, float& y, float& z, 
									bool freeRoam, std::uint8_t account_num)
{
	sound_producer_vector.push_back( std::unique_ptr <SoundProducer>(new SoundProducer()) );

	sound_producer_vector.back()->InitSoundProducer(name,x,y,z);
	sound_producer_vector.back()->SetFreeRoamBool(freeRoam);
	sound_producer_vector.back()->SetAccountNumber(account_num);

	soundproducer_registry.AddRecentSoundProducerMadeToRegistry();
	
	soundproducer_registry.AddSourceOfLastSoundProducerToSoundProducerRegistry();
	
}

void MainGuiEditor::RemoveSoundProducer(int index)
{
	if(index >= 0)
	{
		std::swap( sound_producer_vector[index],sound_producer_vector.back());
		sound_producer_vector.pop_back();

		soundproducer_registry.RemoveThisSourceFromSoundProducerRegistry(index);
		
	}
	
}

void MainGuiEditor::RemoveAllSoundProducersSafely()
{
	soundproducer_registry.RemoveAllSourcesFromSoundProducerRegistry();
	
	sound_producer_vector.clear();
}

/*

void MainFrame::OnSetupSerial(wxCommandEvent& event)
{
	if(listenerPtr == nullptr){std::cout << "Listener Raw Pointer is null in OnSetupSerial. \n";}

	std::unique_ptr <SetupSerialDialog> setupSerialDialog(new SetupSerialDialog( wxT("Setup Serial"),
																		listenerExternalPtr)
															);

    setupSerialDialog->Show(true);
}


*/
