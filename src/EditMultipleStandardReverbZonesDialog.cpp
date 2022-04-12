#include "EditMultipleStandardReverbZonesDialog.h"

#include "global_variables.h"

#include "raygui/gui_valid_float.h"
#include "raygui/gui_dropdown_listview.h"

#include <string>

EditMultipleStandardReverbZonesDialog::EditMultipleStandardReverbZonesDialog()
{
	okClicked = false;
	cancelClicked = false;
	
	m_selection_index = 0;
	spt_selection_index = 0;
}

void EditMultipleStandardReverbZonesDialog::SetCurrentZoneIndexForEditing(size_t index){m_selection_index = index;}

void EditMultipleStandardReverbZonesDialog::SetPointerToEffectsManager(EffectsManager* effects_manager){m_effects_manager_ptr = effects_manager;}

bool EditMultipleStandardReverbZonesDialog::OkClickedOn(){return okClicked;}

bool EditMultipleStandardReverbZonesDialog::CancelClickedOn(){return cancelClicked;}

static std::string standard_reverb_zone_choices = "";

//DropDownListViewSettings InitDropDownListViewSettings(int itemsShowCount, bool editMode, bool valueChanged, int scrollIndex)
static DropDownListViewSettings dropdown_listview_settings = InitDropDownListViewSettings(3,false,false,0);

static int editsr_listview_activeIndex = 0;
static int editsr_listview_itemsCount = 0;


static bool sr_name_box_pressed = false;
static char sr_char_name[20] = "name here";

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

static ValidFloatParamSettings lateReverbDelayValueParam = InitValidFloatParamSettings(0.011f, 0.011f, 0.0f, 0.1f, "-0.011");

static ValidFloatParamSettings airAbsorptionGainValueParam = InitValidFloatParamSettings(0.994f, 0.994f, 0.892f, 1.0f, "0.994");

static ValidFloatParamSettings roomRolloffValueParam = InitValidFloatParamSettings(0.0f, 0.0f, 0.0f, 10.0f, "0.0");

void EditMultipleStandardReverbZonesDialog::InitGUI()
{
	if(m_effects_manager_ptr)
	{
		//clear previous sound choices
		standard_reverb_zone_choices.clear();
				
		for(size_t i = 0; i < m_effects_manager_ptr->standard_reverb_zones_vector.size(); i++ )
		{
			standard_reverb_zone_choices += m_effects_manager_ptr->standard_reverb_zones_vector[i].GetNameString() + ";";
		}
		
		editsr_listview_itemsCount = int(m_effects_manager_ptr->standard_reverb_zones_vector.size());
		
		//initialize values in GUI text boxes based on first choice
		//EditMultipleSoundProducersDialog::SoundProducerSelectedInListBox(current_sound_producer_editing_index);
		EditMultipleStandardReverbZonesDialog::ReverbZoneSelectedInListBox(m_selection_index);
		
	}
}

void EditMultipleStandardReverbZonesDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){180,60,620,540},"Edit Standard Reverb Zone");
	
	if(exit){cancelClicked = true;}
	
	//initialize text fields
	
	if( GuiTextBox((Rectangle){300,130,100,50}, sr_char_name, 20, sr_name_box_pressed) )
	{
		sr_name_box_pressed = !sr_name_box_pressed;
	}
    
    
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){300,200,50,50}, 20, &xValueParam, "X:", 10) )				
	{
		xValueParam.editMode = !xValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){400,200,50,50}, 20, &yValueParam,"Y:",10) )
	{
		yValueParam.editMode = !yValueParam.editMode;
	}
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){500,200,50,50}, 20, &zValueParam,"Z:",10) )
	{
		zValueParam.editMode = !zValueParam.editMode;
	}
    
    if( GuiTextBox_ValidValueFloatSimple((Rectangle){300,270,50,50}, 20, &widthValueParam,"Width:",40) )
	{
		widthValueParam.editMode = !widthValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){300,340,50,50}, 20, &densityValueParam,"Density:",40) )
	{
		densityValueParam.editMode = !densityValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){400,340,50,50}, 20, &diffusionValueParam,"Diffusion:",45) )
	{
		diffusionValueParam.editMode = !diffusionValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){480,340,50,50}, 20, &gainValueParam,"Gain:",25) )
	{
		gainValueParam.editMode = !gainValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){575,340,50,50}, 20, &gainValueHFParam,"Gain HF:",40) )
	{
		gainValueHFParam.editMode = !gainValueHFParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){300,420,50,50}, 20, &decayValueParam,"Decay(s):",45) )
	{
		decayValueParam.editMode = !decayValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){400,420,50,50}, 20, &decayHFValueParam,"Decay HF:",50) )
	{
		decayHFValueParam.editMode = !decayHFValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){520,420,50,50}, 20, &reflectionsGainValueParam,"Reflect_Gain:",65) )
	{
		reflectionsGainValueParam.editMode = !reflectionsGainValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){650,420,50,50}, 20, &reflectionsDelayValueParam,"Reflect_Delay:",75) )
	{
		reflectionsDelayValueParam.editMode = !reflectionsDelayValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){300,500,50,50}, 20, &lateReverbGainValueParam,"Late Gain:",45) )
	{
		lateReverbGainValueParam.editMode = !lateReverbGainValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){400,500,50,50}, 20, &lateReverbDelayValueParam,"Late Delay:",50) )
	{
		lateReverbDelayValueParam.editMode = !lateReverbDelayValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){510,500,50,50}, 20, &airAbsorptionGainValueParam,"Air Absorb:",55) )
	{
		airAbsorptionGainValueParam.editMode = !airAbsorptionGainValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){625,500,50,50}, 20, &roomRolloffValueParam,"Room Rolloff:",60) )
	{
		roomRolloffValueParam.editMode = !roomRolloffValueParam.editMode;
	}
	
	/*
	if( GuiDropdownBox((Rectangle){ 300,100,140,30 }, standard_reverb_zone_choices.c_str(), &editsr_dropDownZoneActive, editsr_dropDownEditMode) )
	{
		editsr_dropDownEditMode = !editsr_dropDownEditMode;
		m_selection_index = editsr_dropDownZoneActive;
		EditMultipleStandardReverbZonesDialog::ReverbZoneSelectedInListBox(m_selection_index);
	}
	*/
	
	editsr_listview_activeIndex = Gui_Dropdown_ListView_Simple(&dropdown_listview_settings, (Rectangle){ 300,100,140,40 }, 
																standard_reverb_zone_choices.c_str(), editsr_listview_itemsCount,
																editsr_listview_activeIndex
															);
											
	if(dropdown_listview_settings.valueChanged )
	{
		m_selection_index = editsr_listview_activeIndex;
		EditMultipleStandardReverbZonesDialog::ReverbZoneSelectedInListBox(m_selection_index);
		dropdown_listview_settings.valueChanged = false;
	}
	
	okClicked = GuiButton( (Rectangle){ 400,560, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 560, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
	if(okClicked)
	{
		EditMultipleStandardReverbZonesDialog::ChangeStandardReverbZoneAttributes();
		
	}
	
	/*
	
    wxStaticText* spPreviewText = new wxStaticText(this, -1, wxT("Echo Zone, Sound Track to Preview:"), wxPoint(40, 20));
    
    //make horizontal box to put names in
	wxBoxSizer* hboxEchoZones = new wxBoxSizer(wxHORIZONTAL);
	
	//list box to contain names of Sound Producers to edit, single selection by default 
	listboxEchoZones = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(100, 20)); 
	
	listboxEchoZones->Bind(wxEVT_LISTBOX,&EditMultipleStandardReverbZonesDialog::EchoZoneSelectedInListBox,this);
	
	//add contents of reverb zones to listbox
	for(size_t i = 0; i < effects_manager_ptr->echo_zones_vector.size(); i++)
	{
		EchoZone* thisEchoZone = effects_manager_ptr->GetPointerToEchoZone(i);
					
		wxString mystring( thisEchoZone->GetNameString() );
		listboxEchoZones->Append(mystring);
	}
	
	//list box to contain names of Sound Producers to edit, single selection by default 
	listboxSoundProducers = new wxListBox(this, wxID_ANY,wxPoint(0, 0), wxSize(100, 20)); 
	
	listboxSoundProducers->Bind(wxEVT_LISTBOX,&EditMultipleStandardReverbZonesDialog::SoundProducerTrackSelectedInListBox,this);
	
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


void EditMultipleStandardReverbZonesDialog::ChangeStandardReverbZoneAttributes()
{
	
	if(m_selection_index != -1)
	{
		if(m_effects_manager_ptr->standard_reverb_zones_vector.size() > 0)
		{
			
			ReverbZone* thisReverbZone = &m_effects_manager_ptr->standard_reverb_zones_vector.at(m_selection_index);
			
			std::string name = std::string(sr_char_name);
			thisReverbZone->SetNameString(name);	
			
			thisReverbZone->SetPositionX(xValueParam.current_value);
			thisReverbZone->SetPositionY(yValueParam.current_value);
			thisReverbZone->SetPositionZ(zValueParam.current_value);
			
			thisReverbZone->ChangeWidth(widthValueParam.current_value);
			
			
			tempStandardReverbProp.flDensity = densityValueParam.current_value;
			tempStandardReverbProp.flDiffusion = diffusionValueParam.current_value;
			tempStandardReverbProp.flGain = gainValueParam.current_value;
			tempStandardReverbProp.flGainHF = gainValueHFParam.current_value;
			tempStandardReverbProp.flDecayTime = decayValueParam.current_value;
			tempStandardReverbProp.flDecayHFRatio = decayHFValueParam.current_value;
			tempStandardReverbProp.flReflectionsDelay = reflectionsDelayValueParam.current_value;
			tempStandardReverbProp.flReflectionsGain = reflectionsGainValueParam.current_value;
			tempStandardReverbProp.flLateReverbDelay = lateReverbDelayValueParam.current_value;
			tempStandardReverbProp.flLateReverbGain = lateReverbGainValueParam.current_value;
			tempStandardReverbProp.flAirAbsorptionGainHF = airAbsorptionGainValueParam.current_value;
			tempStandardReverbProp.flRoomRolloffFactor = roomRolloffValueParam.current_value;
			
			thisReverbZone->ChangeStandardReverbZoneProperties(tempStandardReverbProp);
			
		}
			
	}
	
	
}
	


void EditMultipleStandardReverbZonesDialog::Preview()
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


void EditMultipleStandardReverbZonesDialog::ReverbZoneSelectedInListBox(size_t choice)
{
	
	if(m_effects_manager_ptr->standard_reverb_zones_vector.size() > 0)
	{
		ReverbZone* thisReverbZone = &m_effects_manager_ptr->standard_reverb_zones_vector.at(choice);
		
		tempStandardReverbProp = thisReverbZone->GetStandardReverbZoneProperties();
		
		strncpy(sr_char_name, thisReverbZone->GetNameString().c_str(), 20);
		sr_char_name[19] = '\0';
		
		xValueParam = InitValidFloatParamSettings(thisReverbZone->GetPositionX(), 0.0f, min_position_value, max_position_value, std::to_string(thisReverbZone->GetPositionX()).c_str() );

		yValueParam = InitValidFloatParamSettings(thisReverbZone->GetPositionY(), 0.0f, min_position_value, max_position_value, std::to_string(thisReverbZone->GetPositionY()).c_str());

		zValueParam = InitValidFloatParamSettings(thisReverbZone->GetPositionZ(), 0.0f, min_position_value, max_position_value, std::to_string(thisReverbZone->GetPositionZ()).c_str());

		widthValueParam = InitValidFloatParamSettings(thisReverbZone->GetWidth(), 10.0f, 1.0f, 40.0f, std::to_string(thisReverbZone->GetWidth()).c_str());

		densityValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flDensity, 1.0f, 0.0f, 1.0f, std::to_string(tempStandardReverbProp.flDensity).c_str() );

		diffusionValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flDiffusion, 1.0f, 0.0f, 1.0f, std::to_string(tempStandardReverbProp.flDiffusion).c_str() );

		gainValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flGain, 0.32f, 0.0f, 1.0f, std::to_string(tempStandardReverbProp.flGain).c_str());

		gainValueHFParam = InitValidFloatParamSettings(tempStandardReverbProp.flGainHF, 0.89f, 0.0f, 0.99f, std::to_string(tempStandardReverbProp.flGainHF).c_str());

		decayValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flDecayTime, 1.49f, 0.1f, 20.0f, std::to_string(tempStandardReverbProp.flDecayTime).c_str());

		decayHFValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flDecayHFRatio, 0.83f, 0.1f, 2.0f, std::to_string(tempStandardReverbProp.flDecayHFRatio).c_str());

		reflectionsGainValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flReflectionsGain, 0.05f, 0.0f, 3.16f, std::to_string(tempStandardReverbProp.flReflectionsGain).c_str());

		reflectionsDelayValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flReflectionsDelay, 0.007f, 0.0f, 0.3f, std::to_string(tempStandardReverbProp.flReflectionsDelay).c_str());

		lateReverbGainValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flLateReverbGain, 1.26f, 0.0f, 10.0f, std::to_string(tempStandardReverbProp.flLateReverbGain).c_str());

		lateReverbDelayValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flLateReverbDelay, 0.011f, 0.0f, 0.1f, std::to_string(tempStandardReverbProp.flLateReverbDelay).c_str());

		airAbsorptionGainValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flAirAbsorptionGainHF, 0.994f, 0.892f, 1.0f, std::to_string(tempStandardReverbProp.flAirAbsorptionGainHF).c_str());

		roomRolloffValueParam = InitValidFloatParamSettings(tempStandardReverbProp.flRoomRolloffFactor, 0.0f, 0.0f, 10.0f, std::to_string(tempStandardReverbProp.flRoomRolloffFactor).c_str());
		
	}
	
}

void EditMultipleStandardReverbZonesDialog::SoundProducerTrackSelectedInListBox()
{
	//spt_selection_index = listboxSoundProducers->GetSelection();
}

void EditMultipleStandardReverbZonesDialog::resetConfig()
{
	okClicked = false;
	cancelClicked = false;
}
