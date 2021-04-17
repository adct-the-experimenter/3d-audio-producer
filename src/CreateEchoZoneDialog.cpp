#include "CreateEchoZoneDialog.h"

#include "raygui/raygui.h"

#include "raygui/raygui_extras.h"

CreateEchoZoneDialog::CreateEchoZoneDialog() 
{
	okClicked = false;
	cancelClicked = false;
		
	spt_selection_index = -1;
	
}

void CreateEchoZoneDialog::SetPointerToEffectsManager(EffectsManager* effects_manager){m_effects_manager_ptr = effects_manager;}

std::string& CreateEchoZoneDialog::getNewName(){return name;}

void CreateEchoZoneDialog::getNewPosition(float& x, float& y, float& z)
{
	x = xPosition;
	y = yPosition;
	z = zPosition;
}

float& CreateEchoZoneDialog::getNewWidth(){return width;}
	
EchoZoneProperties& CreateEchoZoneDialog::getNewProperties(){return properties;}

static bool ez_name_box_pressed = false;
static char ez_char_name[20] = "name here";

//InitValidFloatParamSettings(current,default,min,max,initialText)

static ValidFloatParamSettings xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

static ValidFloatParamSettings yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

static ValidFloatParamSettings zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

static ValidFloatParamSettings widthValueParam = InitValidFloatParamSettings(10.0f, 10.0f, 1.0f, 40.0f, "10.0");

static ValidFloatParamSettings delayValueParam = InitValidFloatParamSettings(0.1f, 0.1f, 0.0f, 0.207f, "0.1");

static ValidFloatParamSettings lrDelayValueParam = InitValidFloatParamSettings(0.1f, 0.0f, 0.0f, 0.404f, "0.1");

static ValidFloatParamSettings dampingValueParam = InitValidFloatParamSettings(0.5f, 0.5f, 0.0f, 0.99f, "0.1");

static ValidFloatParamSettings feedbackValueParam = InitValidFloatParamSettings(0.5f, 0.5f, 0.0f, 1.0f, "0.5");

static ValidFloatParamSettings spreadValueParam = InitValidFloatParamSettings(-1.0f, -1.0f, -1.0f, 1.0f, "-1.0");

void CreateEchoZoneDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){300,100,400,500},"Create Echo Zone");
	
	if(exit){cancelClicked = true;}
	
	//initialize text fields
	
	if( GuiTextBox((Rectangle){350,130,100,50}, ez_char_name, 20, ez_name_box_pressed) )
	{
		ez_name_box_pressed = !ez_name_box_pressed;
	}
    
    
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){350,200,50,50}, 20, &xValueParam, "X:", 10) )				
	{
		xValueParam.editMode = !xValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){450,200,50,50}, 20, &yValueParam,"Y:",10) )
	{
		yValueParam.editMode = !yValueParam.editMode;
	}
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){550,200,50,50}, 20, &zValueParam,"Z:",10) )
	{
		zValueParam.editMode = !zValueParam.editMode;
	}
    
    if( GuiTextBox_ValidValueFloatSimple((Rectangle){350,270,50,50}, 20, &widthValueParam,"Width:",40) )
	{
		widthValueParam.editMode = !widthValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){350,340,50,50}, 20, &delayValueParam,"Delay:",40) )
	{
		delayValueParam.editMode = !delayValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){455,340,50,50}, 20, &lrDelayValueParam,"LRDelay:",45) )
	{
		lrDelayValueParam.editMode = !lrDelayValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){350,410,50,50}, 20, &dampingValueParam,"Damping:",45) )
	{
		dampingValueParam.editMode = !dampingValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){455,410,50,50}, 20, &feedbackValueParam,"Feedback",50) )
	{
		feedbackValueParam.editMode = !feedbackValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){550,410,50,50}, 20, &spreadValueParam,"Spread",40) )
	{
		spreadValueParam.editMode = !spreadValueParam.editMode;
	}
	
	okClicked = GuiButton( (Rectangle){ 400, 500, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 500, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
	if(okClicked)
	{
		name = std::string(ez_char_name);
		xPosition = xValueParam.current_value;
		yPosition = yValueParam.current_value;
		zPosition = zValueParam.current_value;
		width = widthValueParam.current_value;
		
		properties.flDamping = dampingValueParam.current_value;
		properties.flDelay = delayValueParam.current_value;
		properties.flFeedback = feedbackValueParam.current_value;
		properties.flLRDelay = lrDelayValueParam.current_value;
		properties.flSpread = spreadValueParam.current_value;
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
	
    
    //initialize Ok and Cancel buttons 
	okButton = new wxButton(this, wxID_ANY, wxT("Ok"), 
							wxDefaultPosition, wxSize(70, 30));
	
	okButton->Bind(wxEVT_BUTTON,&CreateEchoZoneDialog::OnOk,this);
	
	cancelButton = new wxButton(this, wxID_ANY, wxT("Cancel"), 
								wxDefaultPosition, wxSize(70, 30));
	
	cancelButton->Bind(wxEVT_BUTTON,&CreateEchoZoneDialog::OnCancel,this);
	
	previewButton = new wxButton(this, wxID_ANY, wxT("Preview"), 
								wxDefaultPosition, wxSize(70, 30));
	
	previewButton->Bind(wxEVT_BUTTON,&CreateEchoZoneDialog::OnPreview,this);
	*/
}



void CreateEchoZoneDialog::Preview()
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

bool CreateEchoZoneDialog::CancelClickedOn(){return cancelClicked;}

bool CreateEchoZoneDialog::OkClickedOn(){return okClicked;}

void CreateEchoZoneDialog::resetConfig()
{
	okClicked = false;
	cancelClicked = false;
	
	xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -10.0f, 10.0f, "0.0");
	yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -10.0f, 10.0f, "0.0");
	zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -10.0f, 10.0f, "0.0");
	
	widthValueParam = InitValidFloatParamSettings(10.0f, 10.0f, 1.0f, 40.0f, "10.0");
	
	delayValueParam = InitValidFloatParamSettings(0.1f, 0.1f, 0.0f, 0.207f, "0.1");

	lrDelayValueParam = InitValidFloatParamSettings(0.1f, 0.0f, 0.0f, 0.404f, "0.1");

	dampingValueParam = InitValidFloatParamSettings(0.5f, 0.5f, 0.0f, 0.99f, "0.1");

	feedbackValueParam = InitValidFloatParamSettings(0.5f, 0.5f, 0.0f, 1.0f, "0.5");

	spreadValueParam = InitValidFloatParamSettings(-1.0f, -1.0f, -1.0f, 1.0f, "-1.0");
	
	memset(ez_char_name, 0, sizeof(ez_char_name));
	strncpy(ez_char_name, "name here", 20);
	ez_char_name[19] = '\0';
}
