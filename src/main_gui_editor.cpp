

#include "main_gui_editor.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "raygui/raygui.h"

#undef RAYGUI_IMPLEMENTATION

#include "CreateSoundProducerDialog.h"
#include "EditMultipleSoundProducersDialog.h"
#include "EditListenerDialog.h"

#include "immediate_mode_sound_player.h"

#define GUI_FILE_DIALOG_IMPLEMENTATION
#include "raygui/gui_file_dialog.h"



//#include "HRTF-Test-Dialog.h"
//#include "Change-HRTF-Dialog.h"

//#include "setup-serial-dialog.h"


//#include "CreateStandardReverbZoneDialog.h"
//#include "CreateEAXReverbZoneDialog.h"
//#include "CreateEchoZoneDialog.h"

//#include "EditMultipleStandardReverbZonesDialog.h"
//#include "EditMultipleEAXReverbZonesDialog.h"
//#include "EditMultipleEchoZonesDialog.h"


bool init_listener_once = false;

//Gui items to initialize

CreateSoundProducerDialog create_sp_dialog("Create Sound Producer");
EditMultipleSoundProducersDialog edit_sp_dialog("Edit Sound Producer");
EditListenerDialog edit_lt_dialog("Edit Listener");
ImmediateModeSoundPlayer im_sound_player;

MainGuiEditor::MainGuiEditor()
{
	edit_sp_dialog.Init(&sound_producer_vector);
	
}

MainGuiEditor::~MainGuiEditor()
{
    
    //close listener reverb thread here
	
	//effects_manager_ptr->FreeEffects();
	
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


		// Create the main frame window
		std::unique_ptr <MainFrame> thisFrame( new MainFrame("3D Audio Producer",&audio_engine) );
		frame = std::move(thisFrame);


		
		MainGuiEditor::initListener();

		//connect mainframe to listener
		frame->SetListenerReference(listener.get());

		//connect mainframe to external listener
		//frame->SetListenerExternalReference(listenerExternal.get());

		//connect mainframe to soundproducer vector
		frame->SetSoundProducerVectorRef(&sound_producer_vector);

		//connect mainframe to openal soft audio engine
		frame->SetAudioEngineReference(&audio_engine);
		
		
		
		//initialize effects manager
		//effects_manager_ptr = std::unique_ptr <EffectsManager>( new EffectsManager( frame->GetReferenceToSoundProducerTrackManager(), listener.get() ) );
		
		//create thread to check if listener is in reverb zone
		
		//connect mainframe to effects manager
		//frame->SetEffectsManagerReference(effects_manager_ptr.get());
		im_sound_player.SetPointerToSoundBank(&m_sound_bank);
	
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
	
	
}

float distanceToMove = 0.1f;
bool disableHotkeys = false;

bool dialogInUse = false;

// Picking line ray
Ray picker_ray = { 0 };
bool picker_ray_launched = false;

bool editKeyPressed = false;

void MainGuiEditor::HandleEvents()
{
	if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		picker_ray = GetMouseRay(GetMousePosition(), main_camera);
		picker_ray_launched = true;
	}
	else{picker_ray_launched = false;}
	
	//if in sound bank area
	if(GetMouseX() > 620){disableHotkeys = true;}
	else{disableHotkeys = false;}
	
	if(disableHotkeys || dialogInUse){return;}
	
	
	//if w key pressed
	if( IsKeyDown(KEY_W) )
	{
		if(listener){listener->MoveForward(distanceToMove);}
	}
	//if a key pressed
	if( IsKeyDown(KEY_A) )
	{
		if(listener){listener->MoveLeft(distanceToMove);}
	}
	//if s key pressed
	if( IsKeyDown(KEY_S) )
	{
		if(listener){listener->MoveBack(distanceToMove);}
	}
	//if d key pressed
	if( IsKeyDown(KEY_D) )
	{
		if(listener){listener->MoveRight(distanceToMove);}
	}
	//if q key pressed
	if( IsKeyDown(KEY_Q) )
	{
		if(listener){listener->MoveDown(distanceToMove);}
	}
	//if e key pressed
	if( IsKeyDown(KEY_E) )
	{
		if(listener){listener->MoveUp(distanceToMove);}
	}
	
	//if key 1 is pressed
	if(IsKeyDown(KEY_ONE))
	{
		editKeyPressed = true;
	}
	else{editKeyPressed = false;}
	
	/*
		//if b key pressed
		case 66:
		{
			if(sound_producer_vector.size() > 0)
			{
				frame->soundproducertrack_manager_ptr->BrowseAudioForThisSoundProducer(sound_producer_vector.back().get());
			}
			break;
		}
		
		//if i key pressed
		case 73:
		{
			if(sound_producer_vector.size() > 0)
			{	
				if(frame->m_sp_toolbar_combobox->GetSelection() >= 0)
				{
					int selection = frame->m_sp_toolbar_combobox->GetSelection();
					double newZ = sound_producer_vector.at(selection)->GetPositionZ() - 1.0; 
					sound_producer_vector.at(selection)->SetPositionZ(newZ);
				}
			}
			break;
		}
		//j key pressed
		case 74:
		{
			if(sound_producer_vector.size() > 0)
			{
				if(frame->m_sp_toolbar_combobox->GetSelection() >= 0)
				{
					int selection = frame->m_sp_toolbar_combobox->GetSelection();
					double newX = sound_producer_vector.at(selection)->GetPositionX() - 1.0; 
					sound_producer_vector.at(selection)->SetPositionX(newX);
				}
			}
			break;
		}
		//k key pressed
		case 75:
		{
			if(sound_producer_vector.size() > 0)
			{
				if(frame->m_sp_toolbar_combobox->GetSelection() >= 0)
				{
					int selection = frame->m_sp_toolbar_combobox->GetSelection();
					double newZ = sound_producer_vector.at(selection)->GetPositionZ() + 1.0; 
					sound_producer_vector.at(selection)->SetPositionZ(newZ);
				}
			}
			break;
		}
		//L key pressed
		case 76:
		{
			if(sound_producer_vector.size() > 0)
			{
				if(frame->m_sp_toolbar_combobox->GetSelection() >= 0)
				{
					int selection = frame->m_sp_toolbar_combobox->GetSelection();
					double newX = sound_producer_vector.at(selection)->GetPositionX() + 1.0; 
					sound_producer_vector.at(selection)->SetPositionX(newX);
				}
			}
			break;
		}
		//u key pressed
		case 85:
		{
			if(sound_producer_vector.size() > 0)
			{
				if(frame->m_sp_toolbar_combobox->GetSelection() >= 0)
				{
					int selection = frame->m_sp_toolbar_combobox->GetSelection();
					double newY = sound_producer_vector.at(selection)->GetPositionY() - 1.0; 
					sound_producer_vector.at(selection)->SetPositionY(newY);
				}
			}
			break;
		}
		//o key pressed
		case 79:
		{
			if(sound_producer_vector.size() > 0)
			{
				if(frame->m_sp_toolbar_combobox->GetSelection() >= 0)
				{
					int selection = frame->m_sp_toolbar_combobox->GetSelection();
					double newY = sound_producer_vector.at(selection)->GetPositionY() + 1.0; 
					sound_producer_vector.at(selection)->SetPositionY(newY);
				}
			}
			break;
		}
		*/
}

//represents the index of sound producer chosen
//signed integer used so that -1 can be used.
std::int32_t soundproducer_picked = -1;

void MainGuiEditor::logic()
{
	//if picker ray launched
	if(picker_ray_launched)
	{
		bool collision = false;
		//for each soundproducer, check if picker ray hits sound producer
		if(sound_producer_vector.size() > 0)
		{
			for(size_t i = 0; i < sound_producer_vector.size(); i++)
			{
				
				double sp_x,sp_y,sp_z;
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
				if(collision){soundproducer_picked = i; break;}
			}
				
		}
		
		if(!collision){soundproducer_picked = -1;}
	}
	
	//run state for immediate mode sound player
	im_sound_player.RunStateForPlayer();
	
}

void MainGuiEditor::DrawGUI_Items()
{
	
	//draw sound bank
	MainGuiEditor::draw_sound_bank();
	
	//draw object creation/edit menu
	MainGuiEditor::draw_object_creation_menu();
	
	//draw immediate mode sound player
	im_sound_player.DrawGui_Item();
	
	//active sound producer dropdown box
	
	//draw gui dropdownbox for choosing sound producer to manipulate with hot keys
	
	
}

bool dropDownObjectTypeMode = false;
int dropDownObjectTypeActive = 0;
bool objectManipulationState = false;


//state 
enum class GuiState : std::uint8_t { NONE=0, CREATE_SOUND_PRODUCER, EDIT_SOUND_PRODUCER, EDIT_LISTENER };
GuiState g_state = GuiState::NONE;

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
				case 3:{break;}
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
				case 3:{break;}
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
	
	switch(g_state)
	{
		case GuiState::EDIT_LISTENER:
		{
			edit_lt_dialog.DrawDialog();
			
			if(edit_lt_dialog.OkClickedOn() || create_sp_dialog.CancelClickedOn())
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
				double x,y,z;
				create_sp_dialog.getNewPosition(x,y,z);
				bool freeRoam = create_sp_dialog.getFreeRoamBool();
				std::uint8_t account_num =  create_sp_dialog.getAccountNumber();
				frame->CreateSoundProducer(name,x, y, z, freeRoam, account_num);
									
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
			
			if(edit_sp_dialog.OkClickedOn())
			{						
				g_state = GuiState::NONE;
				edit_sp_dialog.resetConfig();
				dialogInUse = false;
			}
			
			if(edit_sp_dialog.CancelClickedOn())
			{
				g_state = GuiState::NONE;
				edit_sp_dialog.resetConfig();
				dialogInUse = false;
			}
			
			break;
		}
		default:{ break;}
	}
}


void MainGuiEditor::draw_sound_bank()
{
	
	GuiDrawRectangle((Rectangle){625,50,200,500}, 1, BLACK, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)) );
	GuiDrawText("Sound Bank", (Rectangle){625,50,125,20}, 1, BLACK);
	GuiDrawText("Sound Name", (Rectangle){625,70,125,20}, 1, BLACK);
	GuiDrawText("File", (Rectangle){700,70,125,20}, 1, BLACK);
	
	m_sound_bank.DrawGui_Item();
	
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

/* My frame constructor */
MainFrame::MainFrame(const std::string& title,
    OpenAlSoftAudioEngine* thisAudioEngine)
{
	MainFrame::SetAudioEngineReference(thisAudioEngine);
	
	
	im_sound_player.SetPointerToSoundProducerRegistry(&soundproducer_registry);
	//create file menu item
	
    //create help menu item

    //create sound producers menu item

    //create the edit multiple sound producers menu item

    //create playback menu item

    //Create hrtf menu item

    //create listener menu item
    
    //create effects menu items
    
    //create and set menu bar with items file and help
	
    
    //initliaze save system
    //save_system_ptr = std::unique_ptr <SaveSystem> (new SaveSystem());
	
    //make toolbar for active sound producer
	
	
    
    //Code to initialize timeline track editor part of GUI

	//timeFrame = new TimelineFrame(this);

	/*
	soundproducertrack_manager_ptr = std::unique_ptr <SoundProducerTrackManager>(new SoundProducerTrackManager("SoundProducer Track Manager",
																		audioEnginePtr->GetReferenceToAudioDevice(),
																		audioEnginePtr->GetReferenceToAudioContext() ) ) ;

	soundproducertrack_manager_ptr->SetReferenceToSoundProducerTrackVector(&m_soundproducer_track_vec);
	
//Initialize listener track
	MainFrame::CreateListenerTrack();

//Initialize SoundProducer Track
	
	MainFrame::CreateFirstSoundProducerTrack();
	
	timeFrame->Layout();
	timeFrame->Show(true); //show the timeframe
	*/
}


void MainFrame::SetSoundProducerVectorRef(std::vector < std::unique_ptr <SoundProducer> > *sound_producer_vector)
{
	sound_producer_vector_ref = sound_producer_vector;

	soundproducer_registry.SetReferenceToSoundProducerVector(sound_producer_vector_ref);
}

void MainFrame::SetListenerReference(Listener* thisListener)
{
	listenerPtr = thisListener;
	//m_listener_track->SetReferenceToListenerToManipulate(listenerPtr);
}

/*
void MainFrame::SetListenerExternalReference(ListenerExternal* thisListenerExternal)
{
	listenerExternalPtr = thisListenerExternal;
	m_listener_track->SetReferenceToExternalListener(listenerExternalPtr);
}
*/

void MainFrame::SetAudioEngineReference(OpenAlSoftAudioEngine* audioEngine){ audioEnginePtr = audioEngine;}

//void MainFrame::SetEffectsManagerReference(EffectsManager* effectsManager){effectsManagerPtr = effectsManager;}

//SoundProducerTrackManager* MainFrame::GetReferenceToSoundProducerTrackManager(){return soundproducertrack_manager_ptr.get();}

/*
void MainFrame::OnIdle(wxIdleEvent &event)
{
    if (!_viewer->isRealized())
        return;

    _viewer->frame();

    event.RequestMore();

    //std::cout << "App is Idle." << std::endl;
}

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close( true ); //close window
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	//show message box with ok icon,
	//window title:About Binaural Audio Editor
	//message:
    wxMessageBox( "Version 1.5 of Binaural Audio Editor. \n This program is for producing and editing binaural audio from mono samples of audio.",
                  "About Binaural Audio Editor", wxOK | wxICON_INFORMATION );
}

void MainFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{

	wxFileDialog fileDlg(this, _("Choose the WAV file"), wxEmptyString, wxEmptyString, _("WAV file|*.wav|All files|*.*"));
	if (fileDlg.ShowModal() == wxID_OK)
	{
		wxString path = fileDlg.GetPath();
		//use this path in your app
	}

}

void MainFrame::OnNewProject(wxCommandEvent& WXUNUSED(event))
{
	MainFrame::CreateNewProject();	
}

void MainFrame::OnSaveProject(wxCommandEvent& WXUNUSED(event))
{
	 MainFrame::SaveProject();
}

void MainFrame::OnLoadProject(wxCommandEvent& WXUNUSED(event))
{
	MainFrame::LoadProject();
}

void MainFrame::CreateNewProject()
{
	//TODO free everything
	MainFrame::UnloadAll();
	
	if(timeFrame)
	{
		delete timeFrame;
		timeFrame = nullptr;
	}
	
	
	
	timeFrame = new TimelineFrame(this);
	
	//Initialize listener track
	MainFrame::CreateListenerTrack();
	
	m_listener_track->SetReferenceToListenerToManipulate(listenerPtr);
	m_listener_track->SetReferenceToExternalListener(listenerExternalPtr);

//Initialize SoundProducer Track
	
	MainFrame::CreateFirstSoundProducerTrack();
	
	timeFrame->Layout();
	timeFrame->Show(true); //show the timeframe
}
    
void MainFrame::SaveProject()
{
	wxFileDialog fileDlg(this, _("Save XML file"), "", "",
                       "XML files (*.xml)|*.xml", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (fileDlg.ShowModal() == wxID_OK)
	{
		
		wxString path = fileDlg.GetPath();
		//use this path in your app
		std::string saveFilePath = std::string(path.mb_str());
		
		//if .xml is not in ending of file name
		if(saveFilePath.substr(saveFilePath.length() - 4,saveFilePath.length() - 1) != ".xml")
		{
			saveFilePath.append(".xml");
			
		}
		
		std::cout << "Input save file path:" << saveFilePath << std::endl;
		
		save_system_ptr->SetSaveFilePath(saveFilePath);
		save_system_ptr->SaveProjectToSetFile(sound_producer_vector_ref,effectsManagerPtr,m_listener_track,listenerPtr);
	}
}
    
void MainFrame::LoadProject()
{
	wxFileDialog fileDlg(this, _("Open XML file"), "", "",
                       "XML files (*.xml)|*.xml", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    
	if (fileDlg.ShowModal() == wxID_OK)
	{
		//Start new project
		MainFrame::CreateNewProject();
		
		//set objects in new projects based on saved data read
		
		wxString path = fileDlg.GetPath();
		//use this path in your app
		std::string loadFilePath = std::string(path.mb_str());
				
		std::cout << "Input open file path:" << loadFilePath << std::endl;
		
		std::vector <SoundProducerSaveData> sound_producer_save_data;
		std::vector <SoundProducerTrackSaveData> ptrSPTracksSaveDataVec;
		std::vector <EchoZoneSaveData> echoZonesSaveData;
		std::vector <StandardReverbZoneSaveData> standardRevZonesSaveData;
		std::vector <EAXReverbZoneSaveData> eaxRevZonesSaveData;
		ListenerTrackSaveData lt_save_data;
		ListenerSaveData listener_data;
		
		load_system_ptr->LoadProject(&sound_producer_save_data,
							   &ptrSPTracksSaveDataVec,
							   &echoZonesSaveData,
							   &standardRevZonesSaveData,
							   &eaxRevZonesSaveData,
							   lt_save_data,
							   listener_data,
							   loadFilePath);
							   
		
		//initialize sound producers from save data
		if(sound_producer_save_data.size() > 0)
		{
			for(size_t i = 0; i < sound_producer_save_data.size(); i++)
			{
				std::string null_str = "";
				ALuint empty_buffer = 0;
				MainFrame::CreateSoundProducer(sound_producer_save_data[i].name, 
												null_str, empty_buffer,
												sound_producer_save_data[i].x, sound_producer_save_data[i].y, sound_producer_save_data[i].z,false);
			}
		}
		
		if(echoZonesSaveData.size() > 0)
		{
			for(size_t i = 0; i < echoZonesSaveData.size(); i++)
			{
				
				effectsManagerPtr->CreateEchoZone(echoZonesSaveData[i].name,
												 echoZonesSaveData[i].x,
												 echoZonesSaveData[i].y,
												 echoZonesSaveData[i].z,
												 echoZonesSaveData[i].width,
												 echoZonesSaveData[i].properties);
		
				//add position attitude transform to root group of nodes
				_rootNode->addChild( (effectsManagerPtr->GetReferenceToEffectZoneVector())->back()->getTransformNode() );
			}
		}
		
		if(standardRevZonesSaveData.size() > 0)
		{
			for(size_t i = 0; i < standardRevZonesSaveData.size(); i++)
			{
				
				effectsManagerPtr->CreateStandardReverbZone(standardRevZonesSaveData[i].name,
												 standardRevZonesSaveData[i].x,
												 standardRevZonesSaveData[i].y,
												 standardRevZonesSaveData[i].z,
												 standardRevZonesSaveData[i].width,
												 standardRevZonesSaveData[i].properties);
		
				//add position attitude transform to root group of nodes
				_rootNode->addChild( (effectsManagerPtr->GetReferenceToEffectZoneVector())->back()->getTransformNode() );
			}
		}
		
		if(eaxRevZonesSaveData.size() > 0)
		{
			for(size_t i = 0; i < eaxRevZonesSaveData.size(); i++)
			{
				
				effectsManagerPtr->CreateEAXReverbZone(eaxRevZonesSaveData[i].name,
												 eaxRevZonesSaveData[i].x,
												 eaxRevZonesSaveData[i].y,
												 eaxRevZonesSaveData[i].z,
												 eaxRevZonesSaveData[i].width,
												 eaxRevZonesSaveData[i].properties);
		
				//add position attitude transform to root group of nodes
				_rootNode->addChild( (effectsManagerPtr->GetReferenceToEffectZoneVector())->back()->getTransformNode() );
			}
		}
		
		//initialize sound producer tracks based on save data
		
		if(ptrSPTracksSaveDataVec.size() > 0)
		{
			for(size_t i = 0; i < ptrSPTracksSaveDataVec.size(); i++)
			{
				
				if(i >= 1)
				{
					//detach sizer containing add/rm soundproducertrack buttons from window, add it back later
					timeFrame->GetTimelineWindow()->GetSizer()->Detach(m_add_rm_box_sizer);

					//create new sound producer track and add it to vector of sound producer tracks in MainFrame
					MainFrame::CreateNewSoundProducerTrack();

					//move add sound producer track button to bottom of new sound producer track

					//add sizer containing add/rm soundproducer track buttons to bottom of window
					timeFrame->GetTimelineWindow()->GetSizer()->Add(m_add_rm_box_sizer);

					timeFrame->Layout();
				}
				
				
				
				m_soundproducer_track_vec.at(i)->LoadSoundProducerTrackSaveData(ptrSPTracksSaveDataVec.at(i));
				
				
				if(ptrSPTracksSaveDataVec.at(i).soundproducer_name != "")
				{
					m_soundproducer_track_vec.at(i)->SelectSoundProducerByName(ptrSPTracksSaveDataVec.at(i).soundproducer_name);
					m_soundproducer_track_vec.at(i)->SetComboBoxToThisSelectionName(ptrSPTracksSaveDataVec.at(i).soundproducer_name);
				}
				
			}
		}
		
		//initialize listener track from save data
		if(m_listener_track)
		{
			m_listener_track->LoadListenerTrackSaveData(lt_save_data);
		}
		
		//initialize listener from save data
		if(listenerPtr)
		{
			listenerPtr->LoadListenerSaveData(listener_data);
		}
		
	}
}



void MainFrame::UnloadAll()
{
	//clear sound producer registry
	soundproducer_registry.ClearAll();
	
	//clear objects in root node
	int total_objs_remove = 5;
	_rootNode->removeChildren(1,total_objs_remove);
	
	//clear vector of sound producers
	sound_producer_vector_ref->clear();
	
	//free effects
	effectsManagerPtr->FreeEffects();
	
	//free contents of sound producer tracks
	m_soundproducer_track_vec.clear();
		
}

void MainFrame::OnCreateSoundProducer(wxCommandEvent& event)
{
	//show message box with ok icon,
	//window title:about hello world
	//message: This is a wxWidgets Helo world sample
    //wxMessageBox( "Create Sound Producer", "Create Sound Producer",wxOK | wxCANCEL |wxICON_INFORMATION );

    std::unique_ptr <CreateSoundProducerDialog> soundProducerNewDialog(new CreateSoundProducerDialog(wxT("Create New Sound Producer"),
																									audioEnginePtr) );
    soundProducerNewDialog->Show(true);

    if(soundProducerNewDialog->OkClickedOn())
    {
		double x,y,z;

		soundProducerNewDialog->getNewPosition(x,y,z);
		std::string name = soundProducerNewDialog->getNewName();
		std::string filePath = soundProducerNewDialog->getSoundFilePath();
		ALuint buffer = soundProducerNewDialog->getBuffer();
		bool freeRoam = soundProducerNewDialog->getFreeRoamBool();
		MainFrame::CreateSoundProducer(name,filePath,buffer,x,y,z,freeRoam);
	}

}
*/

void MainFrame::CreateSoundProducer(std::string& name,double& x, double& y, double& z, 
									bool freeRoam, std::uint8_t account_num)
{
	sound_producer_vector_ref->push_back( std::unique_ptr <SoundProducer>(new SoundProducer()) );

	sound_producer_vector_ref->back()->InitSoundProducer(name,x,y,z);
	sound_producer_vector_ref->back()->SetFreeRoamBool(freeRoam);
	sound_producer_vector_ref->back()->SetAccountNumber(account_num);

	soundproducer_registry.AddRecentSoundProducerMadeToRegistry();
	
	soundproducer_registry.AddSourceOfLastSoundProducerToSoundProducerRegistry();
	//m_sp_toolbar_combobox->Append(name);

	//soundproducer_registry.UpdateAllComboBoxesList();
}

/*
void MainFrame::OnEditMultipleSoundProducers(wxCommandEvent& event)
{
	std::unique_ptr <EditMultipleSoundProducersDialog> soundProducerEditDialog(new EditMultipleSoundProducersDialog( wxT("Edit Sound Producers"),
																													sound_producer_vector_ref,
																													audioEnginePtr)
																				);

    soundProducerEditDialog->Show(true);

}

void MainFrame::OnTestHRTF(wxCommandEvent& event)
{
	std::unique_ptr <HRTFTestDialog> hrtfTestDialog(new HRTFTestDialog( wxT("Test HRTF"),
																		audioEnginePtr)
													);

    hrtfTestDialog->Show(true);

}

void MainFrame::OnChangeHRTF(wxCommandEvent& event)
{
	std::unique_ptr <ChangeHRTFDialog> hrtfChangeDialog(new ChangeHRTFDialog( wxT("Change HRTF"),
																		audioEnginePtr)
													);

    hrtfChangeDialog->Show(true);
}

void MainFrame::OnEditListener(wxCommandEvent& event)
{
	if(listenerPtr == nullptr){std::cout << "Listener Raw Pointer is null in OnEditListener. \n";}

	std::unique_ptr <EditListenerDialog> editListenerDialog(new EditListenerDialog( wxT("Edit Listener"),
																		listenerPtr)
															);

    editListenerDialog->Show(true);
}

void MainFrame::OnSetupSerial(wxCommandEvent& event)
{
	if(listenerPtr == nullptr){std::cout << "Listener Raw Pointer is null in OnSetupSerial. \n";}

	std::unique_ptr <SetupSerialDialog> setupSerialDialog(new SetupSerialDialog( wxT("Setup Serial"),
																		listenerExternalPtr)
															);

    setupSerialDialog->Show(true);
}

void MainFrame::OnCreateStandardReverbZone(wxCommandEvent& event)
{
	//show message box with ok icon,
	//window title:about hello world
	//message: This is a wxWidgets Helo world sample
    //wxMessageBox( "Create Sound Producer", "Create Sound Producer",wxOK | wxCANCEL |wxICON_INFORMATION );

    std::unique_ptr <CreateStandardReverbZoneDialog> reverbZoneNewDialog(new CreateStandardReverbZoneDialog(wxT("Create New Standard Reverb Zone"),
																									effectsManagerPtr) );
    reverbZoneNewDialog->Show(true);

    if(reverbZoneNewDialog->OkClicked())
    {
		double x,y,z,width;
		ReverbStandardProperties properties;
		
		reverbZoneNewDialog->getNewPosition(x,y,z);
		std::string name = reverbZoneNewDialog->getNewName();
		width = reverbZoneNewDialog->getNewWidth();
		properties = reverbZoneNewDialog->getNewProperties();
		
		effectsManagerPtr->CreateStandardReverbZone(name,x,y,z,width,properties);
		
		//add position attitude transform to root group of nodes
		_rootNode->addChild( (effectsManagerPtr->GetReferenceToEffectZoneVector())->back()->getTransformNode() );
	}

}

void MainFrame::OnCreateEAXReverbZone(wxCommandEvent& event)
{
	//show message box with ok icon,
	//window title:about hello world
	//message: This is a wxWidgets Helo world sample
    //wxMessageBox( "Create Sound Producer", "Create Sound Producer",wxOK | wxCANCEL |wxICON_INFORMATION );

    std::unique_ptr <CreateEAXReverbZoneDialog> reverbZoneNewDialog(new CreateEAXReverbZoneDialog(wxT("Create New EAX Reverb Zone"),
																									effectsManagerPtr) );
    reverbZoneNewDialog->Show(true);

    if(reverbZoneNewDialog->OkClicked())
    {
		double x,y,z,width;
		ReverbEAXProperties properties;
		
		reverbZoneNewDialog->getNewPosition(x,y,z);
		std::string name = reverbZoneNewDialog->getNewName();
		width = reverbZoneNewDialog->getNewWidth();
		properties = reverbZoneNewDialog->getNewProperties();
		
		effectsManagerPtr->CreateEAXReverbZone(name,x,y,z,width,properties);
		
		//add position attitude transform to root group of nodes
		_rootNode->addChild( (effectsManagerPtr->GetReferenceToEffectZoneVector())->back()->getTransformNode() );
	}

}

void MainFrame::OnCreateEchoZone(wxCommandEvent& event)
{
	std::unique_ptr <CreateEchoZoneDialog> echoZoneNewDialog(new CreateEchoZoneDialog(wxT("Create New Echo Zone"),
																									effectsManagerPtr) );
    echoZoneNewDialog->Show(true);

    if(echoZoneNewDialog->OkClicked())
    {
		double x,y,z,width;
		EchoZoneProperties properties;
		
		echoZoneNewDialog->getNewPosition(x,y,z);
		std::string name = echoZoneNewDialog->getNewName();
		width = echoZoneNewDialog->getNewWidth();
		properties = echoZoneNewDialog->getNewProperties();
		
		effectsManagerPtr->CreateEchoZone(name,x,y,z,width,properties);
		
		//add position attitude transform to root group of nodes
		_rootNode->addChild( (effectsManagerPtr->GetReferenceToEffectZoneVector())->back()->getTransformNode() );
	}
}

void MainFrame::OnEditMultipleStandardReverbZones(wxCommandEvent& event)
{
	std::unique_ptr <EditMultipleStandardReverbZonesDialog> reverbZoneEditDialog(new EditMultipleStandardReverbZonesDialog( wxT("Edit Reverb Zones"),
																													effectsManagerPtr));
																				

    reverbZoneEditDialog->Show(true);

}

void MainFrame::OnEditMultipleEAXReverbZones(wxCommandEvent& event)
{
	std::unique_ptr <EditMultipleEAXReverbZonesDialog> reverbZoneEditDialog(new EditMultipleEAXReverbZonesDialog( wxT("Edit Reverb Zones"),
																													effectsManagerPtr));
																				

    reverbZoneEditDialog->Show(true);

}

void MainFrame::OnEditMultipleEchoZones(wxCommandEvent& event)
{
	
	std::unique_ptr <EditMultipleEchoZonesDialog> echoZoneEditDialog(new EditMultipleEchoZonesDialog( wxT("Edit Echo Zones"),
																													effectsManagerPtr));																			

    echoZoneEditDialog->Show(true);
}

void MainFrame::OnAddSoundProducerTrack(wxCommandEvent& event)
{
	//move remove sound producer track button to top of new sound producer track

	//detach sizer containing add/rm soundproducertrack buttons from window, add it back later
	timeFrame->GetTimelineWindow()->GetSizer()->Detach(m_add_rm_box_sizer);

	//create new sound producer track and add it to vector of sound producer tracks in MainFrame
	MainFrame::CreateNewSoundProducerTrack();

	//move add sound producer track button to bottom of new sound producer track

	//add sizer containing add/rm soundproducer track buttons to bottom of window
	timeFrame->GetTimelineWindow()->GetSizer()->Add(m_add_rm_box_sizer);

	timeFrame->Layout();

}

void MainFrame::CreateListenerTrack()
{
	m_listener_track = new ListenerTrack("Listener Track");

	int space = 20; //the distance,in pixels, between track and previous item(timeline or previous track)
	double start = -10.0f; //lowest value
	double end = 10.0f; //highest value
	int numTicks = 11; //number of ticks between lowest value and highest value including zero
	double resolution = 1; //the fineness of how much variable can be incremented/decremented by

	double ostart = -1;
	double oend = 1;
	double oresolution = 0.1;

	//initialize listener track
	m_listener_track->InitTrack(timeFrame,nullptr);
	m_listener_track->SetupAxisForPositionVariable(start,end,resolution,numTicks);

	m_listener_track->SetupAxisForOrientationVariable(ostart,oend,oresolution,numTicks);

	//add block of space between Timeline Ruler and Sound Producer Track
	timeFrame->AddSpacerBlock(40);

	//add text to indicate it is a Listener Track
	wxBoxSizer* hboxTextListener = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textListener = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Listener Track"),wxDefaultPosition );
	hboxTextListener->Add(textListener);
	timeFrame->AddBoxSizer(hboxTextListener);

	//add x,y,z tracks of ListenerTrack to time frame

	//add label for x track
	wxBoxSizer* hboxLXText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textLX = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Listener X Position"),wxDefaultPosition );
	hboxLXText->Add(textLX);
	timeFrame->AddBoxSizer(hboxLXText);

	timeFrame->AddTrack(m_listener_track->GetReferenceToXTrack(),space);

	//add label for y track
	wxBoxSizer* hboxLYText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textLY = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Listener Y Position"),wxDefaultPosition );
	hboxLYText->Add(textLY);
	timeFrame->AddBoxSizer(hboxLYText);

	timeFrame->AddTrack(m_listener_track->GetReferenceToYTrack(),space);

	//add label for z track
	wxBoxSizer* hboxLZText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textLZ = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Listener Z Position"),wxDefaultPosition );
	hboxLZText->Add(textLZ);
	timeFrame->AddBoxSizer(hboxLZText);

	timeFrame->AddTrack(m_listener_track->GetReferenceToZTrack(),space);

	//add quat w,x,y,z tracks of ListenerTrack to time frame

	//add label for w quat track
	wxBoxSizer* hboxLQWText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textLQW = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Listener Quaternion W"),wxDefaultPosition );
	hboxLQWText->Add(textLQW);
	timeFrame->AddBoxSizer(hboxLQWText);

	timeFrame->AddTrack(m_listener_track->GetReferenceToQuatWTrack(),space);

	//add label for x quat track
	wxBoxSizer* hboxLQXText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textLQX = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Listener Quaternion X"),wxDefaultPosition );
	hboxLQXText->Add(textLQX);
	timeFrame->AddBoxSizer(hboxLQXText);

	timeFrame->AddTrack(m_listener_track->GetReferenceToQuatXTrack(),space);

	//add label for y quat track
	wxBoxSizer* hboxLQYText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textLQY = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Listener Quaternion Y"),wxDefaultPosition );
	hboxLQYText->Add(textLQY);
	timeFrame->AddBoxSizer(hboxLQYText);

	timeFrame->AddTrack(m_listener_track->GetReferenceToQuatYTrack(),space);

	//add label for z quat track
	wxBoxSizer* hboxLQZText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textLQZ = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Listener Quaternion Z"),wxDefaultPosition );
	hboxLQZText->Add(textLQZ);
	timeFrame->AddBoxSizer(hboxLQZText);

	timeFrame->AddTrack(m_listener_track->GetReferenceToQuatZTrack(),space);


	timeFrame->AddTrackFunctionToCallInTimerLoopPlayState(m_listener_track);

	//add block of space between Sound Producer Track and Listener Track
	timeFrame->AddSpacerBlock(40);
}

void MainFrame::CreateFirstSoundProducerTrack()
{
	int space = 20; //the distance,in pixels, between track and previous item(timeline or previous track)
	double start = -10.0f; //lowest value
	double end = 10.0f; //highest value
	int numTicks = 11; //number of ticks between lowest value and highest value including zero
	double resolution = 1; //the fineness of how much variable can be incremented/decremented by

	double ostart = -1;
	double oend = 1;
	double oresolution = 0.1;
	
	m_soundproducer_track_vec.push_back(new SoundProducerTrack("SoundProducer Track",
											audioEnginePtr->GetReferenceToAudioDevice(),
											audioEnginePtr->GetReferenceToAudioContext())
										);
	soundproducertrack_manager_ptr->AddSourceOfLastTrackToSoundProducerTrackManager();
	
	//initialize sound producer track stuff
	m_soundproducer_track_vec[0]->InitTrack(timeFrame->GetTimelineWindow(),nullptr);

	//initialize audio track
	wxButton* browseButton = new wxButton(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Browse"), wxDefaultPosition, wxSize(70, 30) );
	m_soundproducer_track_vec[0]->GetReferenceToStereoAudioTrack()->SetReferenceToBrowseButton(browseButton);
	m_soundproducer_track_vec[0]->GetReferenceToStereoAudioTrack()->InitTrack(timeFrame->GetTimelineWindow(),nullptr);
	m_soundproducer_track_vec.at(0)->SetReferenceToPlaybackControls(timeFrame->GetPlaybackControlsReference());
	
	wxButton* importAudioDAWButton = new wxButton(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Import DAW Audio"), wxDefaultPosition, wxSize(150, 30) ); 
	m_soundproducer_track_vec.at(0)->SetReferenceToImportAudioDAWButton(importAudioDAWButton);
	
	
	
	//initialize double tracks
	m_soundproducer_track_vec[0]->SetupAxisForVariable(start,end,resolution,numTicks); //setup bounds for vertical axes
	m_soundproducer_track_vec[0]->SetReferenceToSoundProducerRegistry(&soundproducer_registry);
	m_soundproducer_track_vec[0]->UpdateComboBoxListFromSoundProducerRegistry();
	soundproducer_registry.AddReferenceToComboBox(m_soundproducer_track_vec[0]->GetReferenceToComboBox());

	//add text to indicate it is a Sound Producer Track
	wxBoxSizer* hboxTextSPTrack = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textSPTrack = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Sound Producer Track(x,y,z)"),wxDefaultPosition );
	hboxTextSPTrack->Add(textSPTrack);
	hboxTextSPTrack->Add(m_soundproducer_track_vec[0]->GetReferenceToComboBox());
	timeFrame->AddBoxSizer(hboxTextSPTrack);

	sound_producer_track_count = 1;

	double audio_start = 0.0f; //lowest value
	double audio_end = 10.0f; //highest value
	int audio_numTicks = 11; //number of ticks between lowest value and highest value including zero
	double audio_resolution = 0.1; //the fineness of how much variable can be incremented/decremented by

	//setup bounds for vertical axis
	m_soundproducer_track_vec[0]->SetupAxisForAudio(audio_start,audio_end,audio_resolution,audio_numTicks);

	timeFrame->AddSpacerBlock(50);

	//setup browse button and text label for stereo audio track
	wxBoxSizer* hboxButtonText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textButtonText = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Stereo Audio Track"),wxDefaultPosition );
	hboxButtonText->Add(textButtonText);
	hboxButtonText->Add(browseButton);
	hboxButtonText->Add(importAudioDAWButton);
	timeFrame->AddBoxSizer(hboxButtonText);

	//add label for left channel audio
	wxBoxSizer* hboxAudioLeftText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textAudioLeft = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Left Channel Audio"),wxDefaultPosition );
	hboxAudioLeftText->Add(textAudioLeft);
	timeFrame->AddBoxSizer(hboxAudioLeftText);

	//add left channel track to time frame
	timeFrame->AddTrack(m_soundproducer_track_vec[0]->GetReferenceToStereoAudioTrack()->GetReferenceToLeftChannelTrack(),space);

	//add label for right channel audio
	wxBoxSizer* hboxAudioRightText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textAudioRight = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Right Channel Audio"),wxDefaultPosition );
	hboxAudioRightText->Add(textAudioRight);
	timeFrame->AddBoxSizer(hboxAudioRightText);

	//add right channel track to time frame
	timeFrame->AddTrack(m_soundproducer_track_vec[0]->GetReferenceToStereoAudioTrack()->GetReferenceToRightChannelTrack(),space);

	//add track functions to call for sound producer track manager
	timeFrame->AddTrackFunctionToCallInTimerLoopPlayState(soundproducertrack_manager_ptr.get());
	timeFrame->AddTrackFunctionToCallInTimerLoopNullState(soundproducertrack_manager_ptr.get());
	timeFrame->AddTrackFunctionToCallInTimerLoopPauseState(soundproducertrack_manager_ptr.get());
	timeFrame->AddTrackFunctionToCallInTimerLoopRewindState(soundproducertrack_manager_ptr.get());
	timeFrame->AddTrackFunctionToCallInTimerLoopFastForwardState(soundproducertrack_manager_ptr.get());


	//add x,y,z tracks of SoundProducerTrack to time frame

	//add label for x track
	wxBoxSizer* hboxXText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textX = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("X"),wxDefaultPosition );
	hboxXText->Add(textX);
	timeFrame->AddBoxSizer(hboxXText);

	timeFrame->AddTrack(m_soundproducer_track_vec[0]->GetReferenceToXTrack(),space);

	//add label for y track
	wxBoxSizer* hboxYText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textY = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Y"),wxDefaultPosition );
	hboxYText->Add(textY);
	timeFrame->AddBoxSizer(hboxYText);

	timeFrame->AddTrack(m_soundproducer_track_vec[0]->GetReferenceToYTrack(),space);

	//add label for z track
	wxBoxSizer* hboxZText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textZ = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Z"),wxDefaultPosition );
	hboxZText->Add(textZ);
	timeFrame->AddBoxSizer(hboxZText);

	timeFrame->AddTrack(m_soundproducer_track_vec[0]->GetReferenceToZTrack(),space);

	//add block of space between Sound Producer Track and bottom of window, also extends height of window
	timeFrame->AddSpacerBlock(100);

	//intialize add and remove buttons for new soundproducer tracks
	m_add_soundproducertrack_button = new wxButton(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Add SoundProducer Track"), wxDefaultPosition, wxSize(200, 30) );
	m_add_soundproducertrack_button->Bind(wxEVT_BUTTON, &MainFrame::OnAddSoundProducerTrack,this);


	m_remove_soundproducertrack_button = new wxButton(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Remove SoundProducer Track"), wxDefaultPosition, wxSize(200, 30) );
	m_remove_soundproducertrack_button->Bind(wxEVT_BUTTON, &MainFrame::OnRemoveSoundProducerTrack,this);

	m_add_rm_box_sizer = new wxBoxSizer(wxVERTICAL);

	m_add_rm_box_sizer->Add(m_remove_soundproducertrack_button);
	m_add_rm_box_sizer->AddSpacer(25);
	m_add_rm_box_sizer->Add(m_add_soundproducertrack_button);

	timeFrame->AddBoxSizer(m_add_rm_box_sizer);
}

void MainFrame::CreateNewSoundProducerTrack()
{
	std::cout << "Count of items in timeline window size:" << timeFrame->GetTimelineWindow()->GetSizer()->GetItemCount() << std::endl;
	//set title based on how many sound producer tracks there are
	sound_producer_track_count += 1;

	//convert number to string
	std::string result;          // string which will contain the result
	std::ostringstream convert;   // stream used for the conversion
	convert << sound_producer_track_count;      // insert the textual representation of 'Number' in the characters in the stream
	result = convert.str(); // set 'Result' to the contents of the stream

	wxString title = wxString("SoundProducer Track " + result);

	m_soundproducer_track_vec.push_back(new SoundProducerTrack(title,
											audioEnginePtr->GetReferenceToAudioDevice(),
											audioEnginePtr->GetReferenceToAudioContext())
										);
	soundproducertrack_manager_ptr->AddSourceOfLastTrackToSoundProducerTrackManager();

	//initialize sound producer track stuff
	m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->InitTrack(timeFrame->GetTimelineWindow(),nullptr);
	m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->SetReferenceToPlaybackControls(timeFrame->GetPlaybackControlsReference());

	//initialize audio track
	wxButton* browseButton = new wxButton(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Browse"), wxDefaultPosition, wxSize(70, 30) );
	m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->GetReferenceToStereoAudioTrack()->SetReferenceToBrowseButton(browseButton);
	m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->GetReferenceToStereoAudioTrack()->InitTrack(timeFrame->GetTimelineWindow(),nullptr);
	
	wxButton* importAudioDAWButton = new wxButton(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Import DAW Audio"), wxDefaultPosition, wxSize(150, 30) ); 
	m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->SetReferenceToImportAudioDAWButton(importAudioDAWButton);
	
	//initialize double tracks

	int space = 20; //the distance,in pixels, between track and previous item(timeline or previous track)
	double start = -10.0f; //lowest value
	double end = 10.0f; //highest value
	int numTicks = 11; //number of ticks between lowest value and highest value including zero
	double resolution = 1; //the fineness of how much variable can be incremented/decremented by

	m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->SetupAxisForVariable(start,end,resolution,numTicks); //setup bounds for vertical axes
	m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->SetReferenceToSoundProducerRegistry(&soundproducer_registry);
	m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->UpdateComboBoxListFromSoundProducerRegistry();
	soundproducer_registry.AddReferenceToComboBox(m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->GetReferenceToComboBox());

	//add text to indicate it is a Sound Producer Track
	wxBoxSizer* hboxTextSPTrack = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textSPTrack = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Sound Producer Track(x,y,z)"),wxDefaultPosition );
	hboxTextSPTrack->Add(textSPTrack);
	hboxTextSPTrack->Add(m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->GetReferenceToComboBox());
	timeFrame->AddBoxSizer(hboxTextSPTrack);



	double audio_start = 0.0f; //lowest value
	double audio_end = 10.0f; //highest value
	int audio_numTicks = 11; //number of ticks between lowest value and highest value including zero
	double audio_resolution = 0.1; //the fineness of how much variable can be incremented/decremented by

	//setup bounds for vertical axis
	m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->SetupAxisForAudio(audio_start,audio_end,audio_resolution,audio_numTicks);

	timeFrame->AddSpacerBlock(50);

	//setup browse button and text label for stereo audio track
	wxBoxSizer* hboxButtonText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textButtonText = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Stereo Audio Track"),wxDefaultPosition );
	hboxButtonText->Add(textButtonText);
	hboxButtonText->Add(browseButton);
	hboxButtonText->Add(importAudioDAWButton);
	timeFrame->AddBoxSizer(hboxButtonText);

	//add label for left channel audio
	wxBoxSizer* hboxAudioLeftText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textAudioLeft = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Left Channel Audio"),wxDefaultPosition );
	hboxAudioLeftText->Add(textAudioLeft);
	timeFrame->AddBoxSizer(hboxAudioLeftText);

	//add left channel track to time frame
	timeFrame->AddTrack(m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->GetReferenceToStereoAudioTrack()->GetReferenceToLeftChannelTrack(),space);

	//add label for right channel audio
	wxBoxSizer* hboxAudioRightText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textAudioRight = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Right Channel Audio"),wxDefaultPosition );
	hboxAudioRightText->Add(textAudioRight);
	timeFrame->AddBoxSizer(hboxAudioRightText);

	//add right channel track to time frame
	timeFrame->AddTrack(m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->GetReferenceToStereoAudioTrack()->GetReferenceToRightChannelTrack(),space);

	//add x,y,z tracks of SoundProducerTrack to time frame

	//add label for x track
	wxBoxSizer* hboxXText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textX = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("X"),wxDefaultPosition );
	hboxXText->Add(textX);
	timeFrame->AddBoxSizer(hboxXText);

	timeFrame->AddTrack(m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->GetReferenceToXTrack(),space);

	//add label for y track
	wxBoxSizer* hboxYText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textY = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Y"),wxDefaultPosition );
	hboxYText->Add(textY);
	timeFrame->AddBoxSizer(hboxYText);

	timeFrame->AddTrack(m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->GetReferenceToYTrack(),space);

	//add label for z track
	wxBoxSizer* hboxZText = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *textZ = new wxStaticText(timeFrame->GetTimelineWindow(), wxID_ANY, wxT("Z"),wxDefaultPosition );
	hboxZText->Add(textZ);
	timeFrame->AddBoxSizer(hboxZText);

	timeFrame->AddTrack(m_soundproducer_track_vec.at(m_soundproducer_track_vec.size()-1)->GetReferenceToZTrack(),space);

	//add block of space between Sound Producer Track and bottom of window, also extends height of window
	timeFrame->AddSpacerBlock(100);
}

void MainFrame::OnRemoveSoundProducerTrack(wxCommandEvent& event)
{
	size_t itemCount = 	timeFrame->GetTimelineWindow()->GetSizer()->GetItemCount();
	std::cout << "\nItem Count: " << itemCount << std::endl;

	//if item count is more than 25, which means the initial items to not be deleted are in the timeline window.
	if(itemCount > 33)
	{
		//remove x,y,z tracks of soundproducer track added to timelinewindow
		//and remove the combo box and text label
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-2);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-3);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-4);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-5);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-6);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-7);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-8);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-9);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-10);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-11);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-12);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-13);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-14);
		timeFrame->GetTimelineWindow()->GetSizer()->Remove(itemCount-15);

		//resize frame properly
		timeFrame->Layout();

		//destroy last soundproducertrack from vector containing soundproducertracks
		m_soundproducer_track_vec.pop_back();
		soundproducertrack_manager_ptr->RemoveSourceOfLastTrackFromSoundProducerTrackManager();
		soundproducer_registry.RemoveLastComboBoxReference();

		std::cout << "\nItem Count: " << timeFrame->GetTimelineWindow()->GetSizer()->GetItemCount() << std::endl;
	}

}

void MainFrame::OnPopupClick(wxCommandEvent& evt)
{
	//void *data=static_cast<wxMenu *>( evt.GetEventObject() )->GetClientData();
	//switch(evt.GetId())
	//{
	//	case ID_SOMETHING:{break;}
	//	case ID_SOMETHING_ELSE:{break;}
	//}
}

void MainFrame::OnListRightClick(wxListEvent& evt)
{
	void *data = reinterpret_cast<void *>(evt.GetItem().GetData());
	wxMenu menu;
	menu.SetClientData( data );
	menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnPopupClick), NULL, this);
	PopupMenu(&menu);
}

void MainFrame::OnKeyDown(wxKeyEvent &event)
{
#if wxUSE_UNICODE
    int key = event.GetUnicodeKey();
#else
    int key = event.GetKeyCode();
#endif

	std::cout << "KeyDown:" << key << std::endl;

}
*/
