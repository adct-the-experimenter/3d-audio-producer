#include "EditMultipleEAXReverbZonesDialog.h"

#include "global_variables.h"

#include "raygui/raygui_extras.h"

#include <string>

EditMultipleEAXReverbZonesDialog::EditMultipleEAXReverbZonesDialog()
{
	okClicked = false;
	cancelClicked = false;
	
	m_selection_index = 0;
	spt_selection_index = 0;
}

void EditMultipleEAXReverbZonesDialog::SetCurrentZoneIndexForEditing(size_t index){m_selection_index = index;}

void EditMultipleEAXReverbZonesDialog::SetPointerToEffectsManager(EffectsManager* effects_manager){m_effects_manager_ptr = effects_manager;}

bool EditMultipleEAXReverbZonesDialog::OkClickedOn(){return okClicked;}

bool EditMultipleEAXReverbZonesDialog::CancelClickedOn(){return cancelClicked;}

static std::string eax_reverb_zone_choices = "";

//DropDownListViewSettings InitDropDownListViewSettings(int itemsShowCount, bool editMode, bool valueChanged, int scrollIndex)
static DropDownListViewSettings dropdown_listview_settings = InitDropDownListViewSettings(3,false,false,0);

static int editer_listview_activeIndex = 0;
static int editer_listview_itemsCount = 0;

static bool er_name_box_pressed = false;
static char er_char_name[20] = "name here";

//InitValidFloatParamSettings(current,default,min,max,initialText)

static ValidFloatParamSettings xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

static ValidFloatParamSettings yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

static ValidFloatParamSettings zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

static ValidFloatParamSettings widthValueParam = InitValidFloatParamSettings(10.0f, 10.0f, 1.0f, 40.0f, "10.0");

static ValidFloatParamSettings densityValueParam = InitValidFloatParamSettings(1.0f, 1.0f, 0.0f, 1.0f, "1.0");

static ValidFloatParamSettings diffusionValueParam = InitValidFloatParamSettings(1.0f, 1.0f, 0.0f, 1.0f, "1.0");

static ValidFloatParamSettings gainValueParam = InitValidFloatParamSettings(0.32f, 0.32f, 0.0f, 1.0f, "0.32");

static ValidFloatParamSettings gainValueHFParam = InitValidFloatParamSettings(0.5f, 0.89f, 0.0f, 0.99f, "0.89");

static ValidFloatParamSettings decayValueParam = InitValidFloatParamSettings(1.49f, 1.49f, 0.1f, 20.0f, "1.49");

static ValidFloatParamSettings decayHFValueParam = InitValidFloatParamSettings(0.83f, 0.83f, 0.1f, 2.0f, "0.83");

static ValidFloatParamSettings reflectionsGainValueParam = InitValidFloatParamSettings(0.05f, 0.05f, 0.0f, 3.16f, "0.05");

static ValidFloatParamSettings reflectionsDelayValueParam = InitValidFloatParamSettings(0.007f, 0.007f, 0.0f, 0.3f, "0.007");

static ValidFloatParamSettings lateReverbGainValueParam = InitValidFloatParamSettings(1.26f, 1.26f, 0.0f, 10.0f, "1.26");

static ValidFloatParamSettings lateReverbDelayValueParam = InitValidFloatParamSettings(0.011f, 0.011f, 0.0f, 0.1f, "0.011");

static ValidFloatParamSettings airAbsorptionGainValueParam = InitValidFloatParamSettings(0.994f, 0.994f, 0.892f, 1.0f, "0.994");

static ValidFloatParamSettings roomRolloffValueParam = InitValidFloatParamSettings(0.0f, 0.0f, 0.0f, 10.0f, "0.0");


static ValidFloatParamSettings echoTimeValueParam = InitValidFloatParamSettings(0.25f, 0.25f, 0.075f, 0.25f, "0.25");

static ValidFloatParamSettings echoDepthValueParam = InitValidFloatParamSettings(0.0f, 0.0f, 0.0f, 1.0f, "0.0");

static ValidFloatParamSettings modulationTimeValueParam = InitValidFloatParamSettings(0.25f, 0.25f, 0.004f, 4.0f, "0.25");

static ValidFloatParamSettings modulationDepthValueParam = InitValidFloatParamSettings(0.0f, 0.0f, 0.0f, 1.0f, "0.0");

static ValidFloatParamSettings hfReferenceValueParam = InitValidFloatParamSettings(5000.0f,5000.0f,1000.0f,20000.0f,"5000");

static ValidFloatParamSettings lfReferenceValueParam = InitValidFloatParamSettings(250.0f,250.0f,20.0f,1000.0f,"250");

void EditMultipleEAXReverbZonesDialog::InitGUI()
{
	if(m_effects_manager_ptr)
	{
		//clear previous sound choices
		eax_reverb_zone_choices.clear();
				
		for(size_t i = 0; i < m_effects_manager_ptr->eax_reverb_zones_vector.size(); i++ )
		{
			eax_reverb_zone_choices += m_effects_manager_ptr->eax_reverb_zones_vector[i].GetNameString() + ";";
		}
		
		//initialize values in GUI text boxes based on first choice
		//EditMultipleSoundProducersDialog::SoundProducerSelectedInListBox(current_sound_producer_editing_index);
		EditMultipleEAXReverbZonesDialog::ReverbZoneSelectedInListBox(m_selection_index);
		
	}
}

void EditMultipleEAXReverbZonesDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){180,20,620,580},"Edit EAX Reverb Zone");
	
	if(exit){cancelClicked = true;}
	
	//initialize text fields
	
	if( GuiTextBox((Rectangle){300,80,100,50}, er_char_name, 20, er_name_box_pressed) )
	{
		er_name_box_pressed = !er_name_box_pressed;
	}
    
    
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){300,140,50,50}, 20, &xValueParam, "X:", 10) )				
	{
		xValueParam.editMode = !xValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){400,140,50,50}, 20, &yValueParam,"Y:",10) )
	{
		yValueParam.editMode = !yValueParam.editMode;
	}
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){500,140,50,50}, 20, &zValueParam,"Z:",10) )
	{
		zValueParam.editMode = !zValueParam.editMode;
	}
    
    if( GuiTextBox_ValidValueFloatSimple((Rectangle){300,210,50,50}, 20, &widthValueParam,"Width:",40) )
	{
		widthValueParam.editMode = !widthValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){250,280,50,50}, 20, &densityValueParam,"Density:",40) )
	{
		densityValueParam.editMode = !densityValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){350,280,50,50}, 20, &diffusionValueParam,"Diffusion:",45) )
	{
		diffusionValueParam.editMode = !diffusionValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){430,280,50,50}, 20, &gainValueParam,"Gain:",25) )
	{
		gainValueParam.editMode = !gainValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){525,280,50,50}, 20, &gainValueHFParam,"Gain HF:",40) )
	{
		gainValueHFParam.editMode = !gainValueHFParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){300,360,50,50}, 20, &decayValueParam,"Decay(s):",45) )
	{
		decayValueParam.editMode = !decayValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){400,360,50,50}, 20, &decayHFValueParam,"Decay HF:",50) )
	{
		decayHFValueParam.editMode = !decayHFValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){520,360,50,50}, 20, &reflectionsGainValueParam,"Reflect_Gain:",65) )
	{
		reflectionsGainValueParam.editMode = !reflectionsGainValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){650,360,50,50}, 20, &reflectionsDelayValueParam,"Reflect_Delay:",75) )
	{
		reflectionsDelayValueParam.editMode = !reflectionsDelayValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){300,440,50,50}, 20, &lateReverbGainValueParam,"Late Gain:",45) )
	{
		lateReverbGainValueParam.editMode = !lateReverbGainValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){400,440,50,50}, 20, &lateReverbDelayValueParam,"Late Delay:",50) )
	{
		lateReverbDelayValueParam.editMode = !lateReverbDelayValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){510,440,50,50}, 20, &airAbsorptionGainValueParam,"Air Absorb:",55) )
	{
		airAbsorptionGainValueParam.editMode = !airAbsorptionGainValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){625,440,50,50}, 20, &roomRolloffValueParam,"Room Rolloff:",60) )
	{
		roomRolloffValueParam.editMode = !roomRolloffValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){260,510,50,50}, 20, &echoTimeValueParam,"Echo Time:",60) )
	{
		echoTimeValueParam.editMode = !echoTimeValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){375,510,50,50}, 20, &echoDepthValueParam,"Echo Depth:",60) )
	{
		echoDepthValueParam.editMode = !echoDepthValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){480,510,50,50}, 20, &modulationTimeValueParam,"Mod Time:",50) )
	{
		modulationTimeValueParam.editMode = !modulationTimeValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){595,510,50,50}, 20, &modulationDepthValueParam,"Mod Depth:",55) )
	{
		modulationDepthValueParam.editMode = !modulationDepthValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){600,280,50,50}, 20, &hfReferenceValueParam,"HF:",15) )
	{
		hfReferenceValueParam.editMode = !hfReferenceValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){670,280,50,50}, 20, &lfReferenceValueParam,"LF:",15) )
	{
		lfReferenceValueParam.editMode = !lfReferenceValueParam.editMode;
	}
	
	
	
	okClicked = GuiButton( (Rectangle){ 400,570, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 570, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
	/*
	if( GuiDropdownBox((Rectangle){ 300,50,140,30 }, eax_reverb_zone_choices.c_str(), &editer_dropDownZoneActive, editer_dropDownEditMode) )
	{
		editer_dropDownEditMode = !editer_dropDownEditMode;
		m_selection_index = editer_dropDownZoneActive;
		EditMultipleEAXReverbZonesDialog::ReverbZoneSelectedInListBox(m_selection_index);
	}
	*/
	
	editer_listview_activeIndex = Gui_Dropdown_ListView_Simple(&dropdown_listview_settings, (Rectangle){ 300,50,140,40 }, 
																eax_reverb_zone_choices.c_str(), editer_listview_itemsCount,
																editer_listview_activeIndex
															);
											
	if(dropdown_listview_settings.valueChanged )
	{
		m_selection_index = editer_listview_activeIndex;
		EditMultipleEAXReverbZonesDialog::ReverbZoneSelectedInListBox(m_selection_index);
		dropdown_listview_settings.valueChanged = false;
	}
	
	if(okClicked)
	{
		EditMultipleEAXReverbZonesDialog::ChangeEAXReverbZoneAttributes();
		
	}
	
	/*
	
    wxStaticText* spPreviewText = new wxStaticText(this, -1, wxT("Echo Zone, Sound Track to Preview:"), wxPoint(40, 20));
    
    //make horizontal box to put names in
	wxBoxSizer* hboxEchoZones = new wxBoxSizer(wxHORIZONTAL);
	
	//list box to contain names of Sound Producers to edit, single selection by default 
	listboxEchoZones = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(100, 20)); 
	
	listboxEchoZones->Bind(wxEVT_LISTBOX,&EditMultipleEAXReverbZonesDialog::EchoZoneSelectedInListBox,this);
	
	//add contents of reverb zones to listbox
	for(size_t i = 0; i < effects_manager_ptr->echo_zones_vector.size(); i++)
	{
		EchoZone* thisEchoZone = effects_manager_ptr->GetPointerToEchoZone(i);
					
		wxString mystring( thisEchoZone->GetNameString() );
		listboxEchoZones->Append(mystring);
	}
	
	//list box to contain names of Sound Producers to edit, single selection by default 
	listboxSoundProducers = new wxListBox(this, wxID_ANY,wxPoint(0, 0), wxSize(100, 20)); 
	
	listboxSoundProducers->Bind(wxEVT_LISTBOX,&EditMultipleEAXReverbZonesDialog::SoundProducerTrackSelectedInListBox,this);
	
	//add contents of soundproducers to listbox
	if(effects_manager_ptr->GetReferenceToSoundProducerTracksVector()->size() > 0)
	{
		for(size_t i = 0; i < effects_manager_ptr->GetReferenceToSoundProducerTracksVector()->size(); i++)
		{
			SoundProducer* thisSoundProducer =  effects_manager_ptr->GetReferenceToSoundProducerTracksVector()->at(i)->GetReferenceToSoundProducerManipulated();
			if(thisSoundProducer)
			{
				wxString mystring( thisSoundProducer->GetNameString() );
				listboxSoundProducers ->Append(mystring);
			}
		}
	}
	
	
    
	
	applyButton = new wxButton(this, wxID_ANY, wxT("Apply"), 
							wxDefaultPosition, wxSize(70, 30));
		
	
	previewButton = new wxButton(this, wxID_ANY, wxT("Preview"), 
								wxDefaultPosition, wxSize(70, 30));
	*/

	
}


void EditMultipleEAXReverbZonesDialog::ChangeEAXReverbZoneAttributes()
{
	
	if(m_selection_index != -1)
	{
		if(m_effects_manager_ptr->eax_reverb_zones_vector.size() > 0)
		{
			
			ReverbZone* thisReverbZone = &m_effects_manager_ptr->eax_reverb_zones_vector.at(m_selection_index);
			
			std::string name = std::string(er_char_name);
			thisReverbZone->SetNameString(name);	
			
			thisReverbZone->SetPositionX(xValueParam.current_value);
			thisReverbZone->SetPositionY(yValueParam.current_value);
			thisReverbZone->SetPositionZ(zValueParam.current_value);
			
			thisReverbZone->ChangeWidth(widthValueParam.current_value);
			
			
			tempEAXReverbProp.flDensity = densityValueParam.current_value;
			tempEAXReverbProp.flDiffusion = diffusionValueParam.current_value;
			tempEAXReverbProp.flGain = gainValueParam.current_value;
			tempEAXReverbProp.flGainHF = gainValueHFParam.current_value;
			tempEAXReverbProp.flDecayTime = decayValueParam.current_value;
			tempEAXReverbProp.flDecayHFRatio = decayHFValueParam.current_value;
			tempEAXReverbProp.flReflectionsDelay = reflectionsDelayValueParam.current_value;
			tempEAXReverbProp.flReflectionsGain = reflectionsGainValueParam.current_value;
			tempEAXReverbProp.flLateReverbDelay = lateReverbDelayValueParam.current_value;
			tempEAXReverbProp.flLateReverbGain = lateReverbGainValueParam.current_value;
			tempEAXReverbProp.flAirAbsorptionGainHF = airAbsorptionGainValueParam.current_value;
			tempEAXReverbProp.flRoomRolloffFactor = roomRolloffValueParam.current_value;
			
			tempEAXReverbProp.flEchoTime = echoTimeValueParam.current_value;
			tempEAXReverbProp.flEchoDepth = echoDepthValueParam.current_value;
			tempEAXReverbProp.flModulationTime = modulationTimeValueParam.current_value;
			tempEAXReverbProp.flModulationDepth = modulationDepthValueParam.current_value;
			tempEAXReverbProp.flHFReference = hfReferenceValueParam.current_value;
			tempEAXReverbProp.flLFReference = lfReferenceValueParam.current_value;
			
			thisReverbZone->ChangeEAXReverbZoneProperties(tempEAXReverbProp);
			
		}
			
	}
	
	
}
	


void EditMultipleEAXReverbZonesDialog::Preview()
{
	/*
	if(effects_manager_ptr->GetReferenceToSoundProducerTracksVector()->size() > 0)
	{
		
		//get sound producer track of first sound producer track
		std::vector <SoundProducerTrack*> *ref_track_vec = effects_manager_ptr->GetReferenceToSoundProducerTracksVector();
		
		if(spt_selection_index != -1 && m_selection_index != -1)
		{
			SoundProducerTrack* thisTrack = ref_track_vec->at(spt_selection_index);
		
			//if track has a sound producer
			if(thisTrack->GetReferenceToSoundProducerManipulated() != nullptr)
			{
				//Create temporary reverb zone
				EchoZone tempZone;
				
				( textFieldX->GetLineText(0) ).ToDouble(&xPosition);
				( textFieldY->GetLineText(0) ).ToDouble(&yPosition);
				( textFieldZ->GetLineText(0) ).ToDouble(&zPosition);
				( textFieldWidth->GetLineText(0) ).ToDouble(&width);
				
				EchoZone* thisEchoZone = &effects_manager_ptr->echo_zones_vector.at(m_selection_index);
					
				
					( textFieldX->GetLineText(0) ).ToDouble(&xPosition);
					( textFieldY->GetLineText(0) ).ToDouble(&yPosition);
					( textFieldZ->GetLineText(0) ).ToDouble(&zPosition);
					( textFieldWidth->GetLineText(0) ).ToDouble(&width);

					( textField_flDelay->GetLineText(0) ).ToDouble(&tempEchoProp.flDelay);
					( textField_flLRDelay->GetLineText(0) ).ToDouble(&tempEchoProp.flLRDelay);
					( textField_flDamping->GetLineText(0) ).ToDouble(&tempEchoProp.flDamping);
					( textField_flFeedback->GetLineText(0) ).ToDouble(&tempEchoProp.flFeedback);
					( textField_flSpread->GetLineText(0) ).ToDouble(&tempEchoProp.flSpread);

					tempZone.InitEchoZone(name,xPosition,yPosition,zPosition,width,tempEchoProp);
					
				
				//apply effect to sound producer track
				effects_manager_ptr->ApplyThisEffectZoneEffectToThisTrack(thisTrack, &tempZone);
				
				//play track
				effects_manager_ptr->m_track_manager_ptr->PlayThisTrackFromSoundProducerTrackVector(spt_selection_index);
				
				//delay for a few seconds 				
				double duration = 4; //seconds
				long endtime = ::wxGetLocalTime() + duration;
				while( ::wxGetLocalTime() < endtime )
				{	
					::wxMilliSleep(100);
				}
				
				//pause instead of stop to avoid crash with stopping source with effect applied
				effects_manager_ptr->m_track_manager_ptr->PauseThisTrackFromSoundProducerTrackVector(spt_selection_index);
				
				//remove effect from sound producer track
				effects_manager_ptr->RemoveEffectFromThisTrack(thisTrack);
				
				//free effect
				tempZone.FreeEffects();
				
			}
				
				
				
			
			else
			{
				wxMessageBox( wxT("Create a soundproducer. Set it to a track. Load audio to it with browse button!") );	
			}
		}
		else
		{
			wxMessageBox( wxT("Select a soundproducer!") );
		}
	}
	*/
}


void EditMultipleEAXReverbZonesDialog::ReverbZoneSelectedInListBox(size_t choice)
{
	
	if(m_effects_manager_ptr->eax_reverb_zones_vector.size() > 0)
	{
		ReverbZone* thisReverbZone = &m_effects_manager_ptr->eax_reverb_zones_vector.at(choice);
		
		tempEAXReverbProp = thisReverbZone->GetEAXReverbZoneProperties();
		
		strncpy(er_char_name, thisReverbZone->GetNameString().c_str(), 20);
		er_char_name[19] = '\0';
		
		xValueParam = InitValidFloatParamSettings(thisReverbZone->GetPositionX(), 0.0f, min_position_value, max_position_value, std::to_string(thisReverbZone->GetPositionX()).c_str() );

		yValueParam = InitValidFloatParamSettings(thisReverbZone->GetPositionY(), 0.0f, min_position_value, max_position_value, std::to_string(thisReverbZone->GetPositionY()).c_str());

		zValueParam = InitValidFloatParamSettings(thisReverbZone->GetPositionZ(), 0.0f, min_position_value, max_position_value, std::to_string(thisReverbZone->GetPositionZ()).c_str());

		widthValueParam = InitValidFloatParamSettings(thisReverbZone->GetWidth(), 10.0f, 1.0f, 40.0f, std::to_string(thisReverbZone->GetWidth()).c_str());

		densityValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flDensity, 1.0f, 0.0f, 1.0f, std::to_string(tempEAXReverbProp.flDensity).c_str() );

		diffusionValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flDiffusion, 1.0f, 0.0f, 1.0f, std::to_string(tempEAXReverbProp.flDiffusion).c_str() );

		gainValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flGain, 0.32f, 0.0f, 1.0f, std::to_string(tempEAXReverbProp.flGain).c_str());

		gainValueHFParam = InitValidFloatParamSettings(tempEAXReverbProp.flGainHF, 0.89f, 0.0f, 0.99f, std::to_string(tempEAXReverbProp.flGainHF).c_str());

		decayValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flDecayTime, 1.49f, 0.1f, 20.0f, std::to_string(tempEAXReverbProp.flDecayTime).c_str());

		decayHFValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flDecayHFRatio, 0.83f, 0.1f, 2.0f, std::to_string(tempEAXReverbProp.flDecayHFRatio).c_str());

		reflectionsGainValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flReflectionsGain, 0.05f, 0.0f, 3.16f, std::to_string(tempEAXReverbProp.flReflectionsGain).c_str());

		reflectionsDelayValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flReflectionsDelay, 0.007f, 0.0f, 0.3f, std::to_string(tempEAXReverbProp.flReflectionsDelay).c_str());

		lateReverbGainValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flLateReverbGain, 1.26f, 0.0f, 10.0f, std::to_string(tempEAXReverbProp.flLateReverbGain).c_str());

		lateReverbDelayValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flLateReverbDelay, 0.011f, 0.0f, 0.1f, std::to_string(tempEAXReverbProp.flLateReverbDelay).c_str());

		airAbsorptionGainValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flAirAbsorptionGainHF, 0.994f, 0.892f, 1.0f, std::to_string(tempEAXReverbProp.flAirAbsorptionGainHF).c_str());

		roomRolloffValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flRoomRolloffFactor, 0.0f, 0.0f, 10.0f, std::to_string(tempEAXReverbProp.flRoomRolloffFactor).c_str());
		
		echoTimeValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flEchoTime, 0.25f, 0.075f, 0.25f, std::to_string(tempEAXReverbProp.flEchoTime).c_str() );

		echoDepthValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flEchoDepth, 0.0f, 0.0f, 1.0f, std::to_string(tempEAXReverbProp.flEchoDepth).c_str() );

		modulationTimeValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flModulationTime, 0.25f, 0.004f, 4.0f, std::to_string(tempEAXReverbProp.flModulationTime).c_str() );

		modulationDepthValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flModulationDepth, 0.0f, 0.0f, 1.0f, std::to_string(tempEAXReverbProp.flModulationDepth).c_str() );

		hfReferenceValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flHFReference,5000.0f,1000.0f,20000.0f,std::to_string(tempEAXReverbProp.flHFReference).c_str() );

		lfReferenceValueParam = InitValidFloatParamSettings(tempEAXReverbProp.flLFReference,250.0f,20.0f,1000.0f,std::to_string(tempEAXReverbProp.flLFReference).c_str() );
	}
	
}

void EditMultipleEAXReverbZonesDialog::SoundProducerTrackSelectedInListBox()
{
	//spt_selection_index = listboxSoundProducers->GetSelection();
}

void EditMultipleEAXReverbZonesDialog::resetConfig()
{
	okClicked = false;
	cancelClicked = false;
}
