

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


#include "immediate_mode_sound_player.h"

#define GUI_FILE_DIALOG_IMPLEMENTATION
#include "raygui/gui_file_dialog.h"


//#include "setup-serial-dialog.h"



bool init_listener_once = false;

//Gui items to initialize

CreateSoundProducerDialog create_sp_dialog("Create Sound Producer");
EditMultipleSoundProducersDialog edit_sp_dialog("Edit Sound Producer");
EditListenerDialog edit_lt_dialog("Edit Listener");
ImmediateModeSoundPlayer im_sound_player;

HRTFTestDialog hrtf_test_dialog;
ChangeHRTFDialog change_hrtf_dialog;

//dialogs for manipulating effect zones
CreateEchoZoneDialog create_echo_zone_dialog;
EditMultipleEchoZonesDialog edit_echo_zone_dialog;

CreateStandardReverbZoneDialog create_sr_zone_dialog;
EditMultipleStandardReverbZonesDialog edit_sr_zone_dialog;

CreateEAXReverbZoneDialog create_er_zone_dialog;
EditMultipleEAXReverbZonesDialog edit_er_zone_dialog;

//dialog for saving and loading project file
static GuiFileDialogState fileDialogState;
enum class ProjectFileState : std::uint8_t{NONE=0, SAVE,LOAD};
static ProjectFileState proj_file_state;


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
		fileDialogState  = InitGuiFileDialog(420, 310, GetWorkingDirectory(), false);
		fileDialogState.position = {200,200};
		
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

static bool editKeyPressed = false;
static bool deleteKeyPressed = false;

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
	
	if(disableHotkeys || dialogInUse || fileDialogState.fileDialogActive){return;}
	
	
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
			fileDialogState.fileDialogActive = true; //activate file dialog
		}
		//if o key down
		else if(IsKeyDown(KEY_O))
		{
			proj_file_state = ProjectFileState::LOAD;
			fileDialogState.fileDialogActive = true; //activate file dialog
		}
	}
		

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
				collision = CheckCollisionRayBox( picker_ray,
						(BoundingBox){
							(Vector3){ sp_x - cube_width/2, sp_y - cube_width/2, sp_z - cube_width/2 },
							(Vector3){ sp_x + cube_width/2, sp_y + cube_width/2, sp_z + cube_width/2 }}
						);
				
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
			
			std::swap( sound_producer_vector[soundproducer_picked],sound_producer_vector.back());
			sound_producer_vector.pop_back();
			
			soundproducer_registry.RemoveThisSourceFromSoundProducerRegistry(soundproducer_picked);
			
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
	im_sound_player.RunStateForPlayer();
	
	sound_player_active = im_sound_player.PlayerInActiveUse();
		
}

void MainGuiEditor::DrawGUI_Items()
{
	
	//draw immediate mode sound player
	im_sound_player.DrawGui_Item();
	
	//draw sound bank
	MainGuiEditor::draw_sound_bank();
	
	//draw object creation/edit menu
	MainGuiEditor::draw_object_creation_menu();
	
	//active sound producer dropdown box
	
	//draw gui dropdownbox for choosing sound producer to manipulate with hot keys
	
	//draw HRTF edit menu
	MainGuiEditor::draw_hrtf_menu();
	
	//draw save button
	
	//draw load button
	
	MainGuiEditor::draw_project_file_dialog();
}

bool dropDownObjectTypeMode = false;
int dropDownObjectTypeActive = 0;
bool objectManipulationState = false;


//gui state 
enum class GuiState : std::uint8_t { NONE=0, 
									CREATE_SOUND_PRODUCER, EDIT_SOUND_PRODUCER, 
									EDIT_LISTENER, 
									TEST_HRTF, CHANGE_HRTF, 
									CREATE_ECHO_ZONE, EDIT_ECHO_ZONE,
									CREATE_SR_ZONE, EDIT_SR_ZONE,
									CREATE_ER_ZONE, EDIT_ER_ZONE };
									
static GuiState g_state = GuiState::NONE;

void MainGuiEditor::draw_object_creation_menu()
{
	
	if(objectManipulationState){}
	
	//create,edit object menu panel
	//draw rectangle panel on the left
	
	GuiDrawRectangle((Rectangle){20,80,150,160}, 1, BLACK, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)) );
	//GuiPanel( (Rectangle){20,80,125,160} );
	GuiDrawText("Object Creation / Edit", (Rectangle){20,80,125,20}, 1, BLACK);
	
	//draw button create
	bool createObjectClicked = GuiButton( (Rectangle){ 25, 140, 70, 30 }, GuiIconText(RICON_FILE_SAVE, "Create") );
	//draw button edit
	bool editObjectClicked = GuiButton( (Rectangle){ 25, 180, 70, 30 }, GuiIconText(RICON_FILE_SAVE, "Edit") );
	
	//draw GuiDropdownBox for choosing type to manipulate
	
	if( GuiDropdownBox((Rectangle){ 25,100,140,30 }, "None;Listener;Sound Producer;Standard Reverb Zone; EAX Reverb Zone; Echo Zone", &dropDownObjectTypeActive, dropDownObjectTypeMode) )
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
				case 2:{ g_state = GuiState::CREATE_SOUND_PRODUCER; dialogInUse = true; break;}
				//standard reverb zone
				case 3:{ g_state = GuiState::CREATE_SR_ZONE; dialogInUse = true; break;}
				//eax reverb zone
				case 4:{ g_state = GuiState::CREATE_ER_ZONE; dialogInUse = true; break;}
				//echo zone
				case 5:{ g_state = GuiState::CREATE_ECHO_ZONE; dialogInUse = true; break;}
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
					g_state = GuiState::EDIT_LISTENER;
					dialogInUse = true;
					edit_lt_dialog.InitGUI();
					break;
				}
				//sound producer
				case 2:
				{ 
					g_state = GuiState::EDIT_SOUND_PRODUCER; 
					dialogInUse = true;
					edit_sp_dialog.SetPointerToSoundBank(&m_sound_bank);
					edit_sp_dialog.InitGUI();
					break;
				}
				//standard reverb zone
				case 3:
				{ 
					g_state = GuiState::EDIT_SR_ZONE;
					dialogInUse = true;
					edit_sr_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
					edit_sr_zone_dialog.InitGUI();
					break;
				}
				//eax reverb zone
				case 4:
				{
					g_state = GuiState::EDIT_ER_ZONE;
					dialogInUse = true;
					edit_er_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
					edit_er_zone_dialog.InitGUI();
					break;
				}
				//echo zone
				case 5:
				{ 
					g_state = GuiState::EDIT_ECHO_ZONE; 
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
		g_state = GuiState::EDIT_SOUND_PRODUCER;
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
				g_state = GuiState::EDIT_SR_ZONE;
				edit_sr_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
				edit_sr_zone_dialog.SetCurrentZoneIndexForEditing(size_t(effect_zone_picked));
				edit_sr_zone_dialog.InitGUI(); 
				break;
			}
			case EffectsManager::EffectZoneType::EAX_REVERB:
			{
				g_state = GuiState::EDIT_ER_ZONE;
				edit_er_zone_dialog.SetPointerToEffectsManager(effects_manager_ptr.get());
				edit_er_zone_dialog.SetCurrentZoneIndexForEditing(size_t(effect_zone_picked));
				edit_er_zone_dialog.InitGUI();
				break;
			}
			case EffectsManager::EffectZoneType::ECHO:
			{
				g_state = GuiState::EDIT_ECHO_ZONE; 
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
		case GuiState::EDIT_LISTENER:
		{
			edit_lt_dialog.DrawDialog();
			
			if(edit_lt_dialog.OkClickedOn() || edit_lt_dialog.CancelClickedOn())
			{
				g_state = GuiState::NONE;
				create_sp_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case GuiState::CREATE_SOUND_PRODUCER:
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
									
				g_state = GuiState::NONE;
				create_sp_dialog.resetConfig();
				dialogInUse = false;
			}
			
			if(create_sp_dialog.CancelClickedOn())
			{
				g_state = GuiState::NONE;
				create_sp_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case GuiState::EDIT_SOUND_PRODUCER:
		{
			edit_sp_dialog.DrawDialog();
			
			if(edit_sp_dialog.OkClickedOn() || edit_sp_dialog.CancelClickedOn())
			{						
				g_state = GuiState::NONE;
				edit_sp_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case GuiState::CREATE_ECHO_ZONE:
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
				
				g_state = GuiState::NONE;
				create_echo_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			if(create_echo_zone_dialog.CancelClickedOn())
			{
				g_state = GuiState::NONE;
				create_echo_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case GuiState::EDIT_ECHO_ZONE:
		{
			edit_echo_zone_dialog.DrawDialog();
			
			if(edit_echo_zone_dialog.OkClickedOn() || edit_echo_zone_dialog.CancelClickedOn())
			{						
				g_state = GuiState::NONE;
				edit_echo_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case GuiState::CREATE_SR_ZONE:
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
				
				g_state = GuiState::NONE;
				create_sr_zone_dialog.resetConfig();
				dialogInUse = false;
			}

			if(create_sr_zone_dialog.CancelClickedOn())
			{
				g_state = GuiState::NONE;
				create_sr_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case GuiState::EDIT_SR_ZONE:
		{
			edit_sr_zone_dialog.DrawDialog();
			
			if(edit_sr_zone_dialog.OkClickedOn() || edit_sr_zone_dialog.CancelClickedOn())
			{						
				g_state = GuiState::NONE;
				edit_sr_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			break;
		}
		case GuiState::CREATE_ER_ZONE:
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
				
				g_state = GuiState::NONE;
				create_sr_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			if(create_er_zone_dialog.CancelClickedOn())
			{
				g_state = GuiState::NONE;
				create_er_zone_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		case GuiState::EDIT_ER_ZONE:
		{
			edit_er_zone_dialog.DrawDialog();
			
			if(edit_er_zone_dialog.OkClickedOn() || edit_er_zone_dialog.CancelClickedOn())
			{						
				g_state = GuiState::NONE;
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
	
	float leftX = GetScreenWidth() - 200;
	
	GuiDrawRectangle((Rectangle){leftX,50,200,500}, 1, BLACK, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)) );
	GuiDrawText("Sound Bank", (Rectangle){leftX,50,125,20}, 1, BLACK);
	GuiDrawText("Sound Name", (Rectangle){leftX,70,125,20}, 1, BLACK);
	GuiDrawText("File", (Rectangle){leftX + 75,70,125,20}, 1, BLACK);
	
	m_sound_bank.DrawGui_Item();
	
}

void MainGuiEditor::draw_hrtf_menu()
{
	GuiDrawRectangle((Rectangle){20,10,150,60}, 1, BLACK, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)) );
	GuiDrawText("HRTF", (Rectangle){20,10,125,20}, 1, BLACK);
	
	//draw button Change
	bool changeHRTFButtonClicked = GuiButton( (Rectangle){ 25, 30, 70, 30 }, GuiIconText(RICON_FILE_SAVE, "Change") );
	//draw button Test
	bool testHRTFButtonClicked = GuiButton( (Rectangle){ 95, 30, 70, 30 }, GuiIconText(RICON_FILE_SAVE, "Test") );
	
	
	if(testHRTFButtonClicked)
	{
		g_state = GuiState::TEST_HRTF;
		dialogInUse = true;
		hrtf_test_dialog.SetPointerToAudioEngine(&audio_engine);
		hrtf_test_dialog.InitGUI();
	}
	
	if(changeHRTFButtonClicked)
	{
		g_state = GuiState::CHANGE_HRTF;
		dialogInUse = true;
		change_hrtf_dialog.SetPointerToAudioEngine(&audio_engine);
		change_hrtf_dialog.InitGUI();
	}
	
	switch(g_state)
	{
		case GuiState::TEST_HRTF:
		{
			hrtf_test_dialog.DrawDialog();
			
			if(hrtf_test_dialog.OkClickedOn())
			{
				g_state = GuiState::NONE;
				dialogInUse = false;
				hrtf_test_dialog.resetConfig();
			}
			break;
		}
		case GuiState::CHANGE_HRTF:
		{
			change_hrtf_dialog.DrawDialog();
			
			if(change_hrtf_dialog.OkClickedOn())
			{
				g_state = GuiState::NONE;
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
	if (fileDialogState.fileDialogActive){ GuiLock();}
	
	if(proj_file_state == ProjectFileState::LOAD)
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
		if (fileDialogState.SelectFilePressed)
		{
			
			// save project file (if supported extension)
			
			//if file name was put in text box
			if (IsFileExtension(fileDialogState.fileNameTextBoxInputCopy, ".xml") )
			{
				char projectFile[512] = { 0 };
				
				strcpy(projectFile, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameTextBoxInputCopy));
				std::string filepath = std::string(projectFile);
				
				std::cout << "save filepath in main gui editor: " << filepath << std::endl;
				
				//save project
				MainGuiEditor::SaveProject(filepath);
			}
			//else if file name was selected
			else if(IsFileExtension(fileDialogState.fileNameText, ".xml"))
			{
				char projectFile[512] = { 0 };
				
				strcpy(projectFile, TextFormat("%s/%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
				std::string filepath = std::string(projectFile);
				
				std::cout << "save filepath in main gui editor: " << filepath << std::endl;
				
				//save project
				MainGuiEditor::SaveProject(filepath);
			}

			fileDialogState.SelectFilePressed = false;
		}
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
	sound_producer_vector.clear();
}

void MainGuiEditor::CreateNewProject()
{
	//free everything
	MainGuiEditor::UnloadAll();
	
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
	save_system_ptr->SaveProjectToSetFile(&sound_producer_vector,effects_manager_ptr.get(),listener.get());
	
}

void MainGuiEditor::LoadProject(std::string& filepath)
{
	
	//Start new project
	MainGuiEditor::CreateNewProject();
	
	//set objects in new projects based on saved data read
	
			
	std::cout << "Input project load file path:" << filepath << std::endl;
	
	std::vector <SoundProducerSaveData> sound_producer_save_data;

	std::vector <EchoZoneSaveData> echoZonesSaveData;
	std::vector <StandardReverbZoneSaveData> standardRevZonesSaveData;
	std::vector <EAXReverbZoneSaveData> eaxRevZonesSaveData;

	ListenerSaveData listener_data;
	
	load_system_ptr->LoadProject(&sound_producer_save_data,
						   &echoZonesSaveData,
						   &standardRevZonesSaveData,
						   &eaxRevZonesSaveData,
						   listener_data,
						   filepath);
						   
	
	//initialize sound producers from save data
	if(sound_producer_save_data.size() > 0)
	{
		for(size_t i = 0; i < sound_producer_save_data.size(); i++)
		{
			//create sound producer
			bool freeRoam = false;
			std::uint8_t account_num =  0;
			MainGuiEditor::CreateSoundProducer(sound_producer_save_data[i].name,
												sound_producer_save_data[i].x, sound_producer_save_data[i].y, sound_producer_save_data[i].z, 
												freeRoam, account_num);
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
