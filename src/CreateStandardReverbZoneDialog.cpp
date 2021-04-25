#include "CreateStandardReverbZoneDialog.h"

#include "raygui/raygui.h"

#include "raygui/raygui_extras.h"

CreateStandardReverbZoneDialog::CreateStandardReverbZoneDialog() 
{
	okClicked = false;
	cancelClicked = false;
		
	spt_selection_index = -1;
	
}

void CreateStandardReverbZoneDialog::SetPointerToEffectsManager(EffectsManager* effects_manager){m_effects_manager_ptr = effects_manager;}

std::string& CreateStandardReverbZoneDialog::getNewName(){return name;}

void CreateStandardReverbZoneDialog::getNewPosition(float& x, float& y, float& z)
{
	x = xPosition;
	y = yPosition;
	z = zPosition;
}

float& CreateStandardReverbZoneDialog::getNewWidth(){return width;}
	
ReverbStandardProperties& CreateStandardReverbZoneDialog::getNewProperties(){return properties;}

static bool sr_name_box_pressed = false;
static char sr_char_name[20] = "name here";

//InitValidFloatParamSettings(current,default,min,max,initialText)

static ValidFloatParamSettings xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

static ValidFloatParamSettings yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

static ValidFloatParamSettings zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

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

void CreateStandardReverbZoneDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){180,80,620,520},"Create Standard Reverb Zone");
	
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
	
	okClicked = GuiButton( (Rectangle){ 400,560, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 560, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
	if(okClicked)
	{
		name = std::string(sr_char_name);
		xPosition = xValueParam.current_value;
		yPosition = yValueParam.current_value;
		zPosition = zValueParam.current_value;
		width = widthValueParam.current_value;
		
		
		properties.flDensity = densityValueParam.current_value;
		properties.flDiffusion = diffusionValueParam.current_value;
		properties.flGain = gainValueParam.current_value;
		properties.flGainHF = gainValueHFParam.current_value;
		properties.flDecayTime = decayValueParam.current_value;
		properties.flDecayHFRatio = decayHFValueParam.current_value;
		properties.flReflectionsDelay = reflectionsDelayValueParam.current_value;
		properties.flReflectionsGain = reflectionsGainValueParam.current_value;
		properties.flLateReverbDelay = lateReverbDelayValueParam.current_value;
		properties.flLateReverbGain = lateReverbGainValueParam.current_value;
		properties.flAirAbsorptionGainHF = airAbsorptionGainValueParam.current_value;
		properties.flRoomRolloffFactor = roomRolloffValueParam.current_value;
		
	}
    /*
	
	//add contents of soundproducers to listbox
	if(m_effects_manager_ptr->GetReferenceToSoundProducerTracksVector()->size() > 0)
	{
		for(size_t i = 0; i < m_effects_manager_ptr->GetReferenceToSoundProducerTracksVector()->size(); i++)
		{
			SoundProducer* thisSoundProducer =  m_effects_manager_ptr->GetReferenceToSoundProducerTracksVector()->at(i)->GetReferenceToSoundProducerManipulated();
			if(thisSoundProducer)
			{
				wxString mystring( thisSoundProducer->GetNameString() );
				listboxSoundProducers ->Append(mystring);
			}
		}
	}
	
	
	previewButton = new wxButton(this, wxID_ANY, wxT("Preview"), 
								wxDefaultPosition, wxSize(70, 30));
	
	previewButton->Bind(wxEVT_BUTTON,&CreateStandardReverbZoneDialog::OnPreview,this);
	*/
}



void CreateStandardReverbZoneDialog::Preview()
{
	/*
	if(m_effects_manager_ptr->GetReferenceToSoundProducerTracksVector()->size() > 0)
	{
		
		//get sound producer track of first sound producer track
		std::vector <SoundProducerTrack*> *ref_track_vec = m_effects_manager_ptr->GetReferenceToSoundProducerTracksVector();
		
		if(spt_selection_index != -1)
		{
			SoundProducerTrack* thisTrack = ref_track_vec->at(spt_selection_index);
		
			//if track has a sound producer
			if(thisTrack->GetReferenceToSoundProducerManipulated() != nullptr)
			{
				//Create temporary reverb zone
				EchoZone tempZone;
				
				name = textFieldName->GetLineText(0).ToStdString();
				( textFieldX->GetLineText(0) ).ToDouble(&xPosition);
				( textFieldY->GetLineText(0) ).ToDouble(&yPosition);
				( textFieldZ->GetLineText(0) ).ToDouble(&zPosition);
				( textFieldWidth->GetLineText(0) ).ToDouble(&width);
				
				( textField_flDelay->GetLineText(0) ).ToDouble(&properties.flDelay);
				( textField_flLRDelay->GetLineText(0) ).ToDouble(&properties.flLRDelay);
				( textField_flDamping->GetLineText(0) ).ToDouble(&properties.flDamping);
				( textField_flFeedback->GetLineText(0) ).ToDouble(&properties.flFeedback);
				( textField_flSpread->GetLineText(0) ).ToDouble(&properties.flSpread);
				
				tempZone.InitEchoZone(name,xPosition,yPosition,zPosition,width,properties);
				
				//apply effect to sound producer track
				m_effects_manager_ptr->ApplyThisEffectZoneEffectToThisTrack(thisTrack, &tempZone);
				
				//play track

				m_effects_manager_ptr->m_track_manager_ptr->PlayThisTrackFromSoundProducerTrackVector(spt_selection_index);
				
				//delay for a few seconds 				
				double duration = 4; //seconds
				long endtime = ::wxGetLocalTime() + duration;
				while( ::wxGetLocalTime() < endtime )
				{	
					::wxMilliSleep(100);
				}
				
				//pause instead of stop to avoid crash with stopping source with effect applied
				m_effects_manager_ptr->m_track_manager_ptr->PauseThisTrackFromSoundProducerTrackVector(spt_selection_index);
				
				//remove effect from sound producer track
				m_effects_manager_ptr->RemoveEffectFromThisTrack(thisTrack);
				
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

bool CreateStandardReverbZoneDialog::CancelClickedOn(){return cancelClicked;}

bool CreateStandardReverbZoneDialog::OkClickedOn(){return okClicked;}

void CreateStandardReverbZoneDialog::resetConfig()
{
	okClicked = false;
	cancelClicked = false;
	
	xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -10.0f, 10.0f, "0.0");
	yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -10.0f, 10.0f, "0.0");
	zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -10.0f, 10.0f, "0.0");
	
	widthValueParam = InitValidFloatParamSettings(10.0f, 10.0f, 1.0f, 40.0f, "10.0");
	
	densityValueParam = InitValidFloatParamSettings(1.0f, 1.0f, 0.0f, 1.0f, "1.0");

	diffusionValueParam = InitValidFloatParamSettings(1.0f, 1.0f, 0.0f, 1.0f, "1.0");

	gainValueParam = InitValidFloatParamSettings(0.32f, 0.32f, 0.0f, 1.0f, "0.32");

	gainValueHFParam = InitValidFloatParamSettings(0.5f, 0.89f, 0.0f, 0.99f, "0.89");

	decayValueParam = InitValidFloatParamSettings(1.49f, 1.49f, 0.1f, 20.0f, "1.49");

	decayHFValueParam = InitValidFloatParamSettings(0.83f, 0.83f, 0.1f, 2.0f, "0.83");

	reflectionsGainValueParam = InitValidFloatParamSettings(0.05f, 0.05f, 0.0f, 3.16f, "0.05");

	reflectionsDelayValueParam = InitValidFloatParamSettings(0.007f, 0.007f, 0.0f, 0.3f, "0.007");

	lateReverbGainValueParam = InitValidFloatParamSettings(1.26f, 1.26f, 0.0f, 10.0f, "1.26");

	lateReverbDelayValueParam = InitValidFloatParamSettings(0.011f, 0.011f, 0.0f, 0.1f, "-0.011");

	airAbsorptionGainValueParam = InitValidFloatParamSettings(0.994f, 0.994f, 0.892f, 1.0f, "0.994");

	roomRolloffValueParam = InitValidFloatParamSettings(0.0f, 0.0f, 0.0f, 10.0f, "0.0");
	
	memset(sr_char_name, 0, sizeof(sr_char_name));
	strncpy(sr_char_name, "name here", 20);
	sr_char_name[19] = '\0';
}
