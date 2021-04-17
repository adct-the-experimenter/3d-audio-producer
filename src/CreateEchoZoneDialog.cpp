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

ValidFloatParamSettings xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -10.0f, 10.0f, "0.0");

static float editez_y_value = 0;
static bool editez_y_box_pressed = false;
static char ez_textBufferY[9] = "0";
static bool ez_yValueChanged = false;

static float editez_z_value = 0;
static bool editez_z_box_pressed = false;
static char ez_textBufferZ[9] = "0";
static bool ez_zValueChanged = false;

static float editez_width_value = 1.0;
static bool editez_width_box_pressed = false;
static char ez_textBufferWidth[9] = "1.0";
static bool ez_widthValueChanged = false;

static float editez_delay_value = 0.1;
static bool editez_delay_box_pressed = false;
static char ez_textBufferDelay[9] = "0.1";
static bool ez_delayValueChanged = false;

static float editez_lrdelay_value = 0.1;
static bool editez_lrdelay_box_pressed = false;
static char ez_textBufferLRDelay[9] = "0.1";
static bool ez_lrdelayValueChanged = false;

static float editez_damping_value = 0.5;
static bool editez_damping_box_pressed = false;
static char ez_textBufferDamping[9] = "0.5";
static bool ez_dampingValueChanged = false;

static float editez_feedback_value = 0.5;
static bool editez_feedback_box_pressed = false;
static char ez_textBufferFeedback[9] = "0.5";
static bool ez_feedbackValueChanged = false;

static float editez_spread_value = 0.5;
static bool editez_spread_box_pressed = false;
static char ez_textBufferSpread[9] = "0.5";
static bool ez_spreadValueChanged = false;

void CreateEchoZoneDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){300,100,400,500},"Create Echo Zone");
	
	if(exit){cancelClicked = true;}
	
	//initialize text fields
	
	if( GuiTextBox((Rectangle){350,130,100,50}, ez_char_name, 20, ez_name_box_pressed) )
	{
		ez_name_box_pressed = !ez_name_box_pressed;
	}
    
    //if( GuiTextBox_ValidValueFloat((Rectangle){350,200,50,50}, 20, editez_x_box_pressed, 
	//								&editez_x_value, 0.0f, -10.0f, 10.0f,
	//								 ez_textBufferX,&ez_xValueChanged,"X:",10) )
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){350,200,50,50}, 20, &xValueParam, "X:", 10) )				
	{
		xValueParam.editMode = !xValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloat((Rectangle){450,200,50,50}, 20, editez_y_box_pressed, 
									&editez_y_value, 0.0f, -10.0f, 10.0f,
									 ez_textBufferY,&ez_yValueChanged,"Y:",10) )
	{
		editez_y_box_pressed = !editez_y_box_pressed;
	}
	if( GuiTextBox_ValidValueFloat((Rectangle){550,200,50,50}, 20, editez_z_box_pressed, 
									&editez_z_value, 0.0f, -10.0f, 10.0f,
									 ez_textBufferZ,&ez_zValueChanged,"Z:",10) )
	{
		editez_z_box_pressed = !editez_z_box_pressed;
	}
    
    if( GuiTextBox_ValidValueFloat((Rectangle){350,270,50,50}, 20, editez_width_box_pressed, 
									&editez_width_value, 0.0f, 1.0f, 30.0f,
									 ez_textBufferWidth,&ez_widthValueChanged,"Width:",40) )
	{
		editez_width_box_pressed = !editez_width_box_pressed;
	}
	
	if( GuiTextBox_ValidValueFloat((Rectangle){350,340,50,50}, 20, editez_delay_box_pressed, 
									&editez_delay_value, 0.1f, 0.0f, 0.207f,
									 ez_textBufferDelay,&ez_delayValueChanged,"Delay:",40) )
	{
		editez_delay_box_pressed = !editez_delay_box_pressed;
	}
	
	if( GuiTextBox_ValidValueFloat((Rectangle){455,340,50,50}, 20, editez_lrdelay_box_pressed, 
									&editez_lrdelay_value, 0.1f, 0.0f, 0.407f,
									 ez_textBufferLRDelay,&ez_lrdelayValueChanged,"LRDelay:",45) )
	{
		editez_lrdelay_box_pressed = !editez_lrdelay_box_pressed;
	}
	
	if( GuiTextBox_ValidValueFloat((Rectangle){350,410,50,50}, 20, editez_damping_box_pressed, 
									&editez_damping_value, 0.5f, 0.0f, 0.99f,
									 ez_textBufferDamping,&ez_dampingValueChanged,"Damping:",45) )
	{
		editez_damping_box_pressed = !editez_damping_box_pressed;
	}
	
	if( GuiTextBox_ValidValueFloat((Rectangle){455,410,50,50}, 20, editez_feedback_box_pressed, 
									&editez_feedback_value, 0.5f, 0.0f, 1.00f,
									 ez_textBufferFeedback,&ez_feedbackValueChanged,"Feedback",50) )
	{
		editez_feedback_box_pressed = !editez_feedback_box_pressed;
	}
	
	if( GuiTextBox_ValidValueFloat((Rectangle){550,410,50,50}, 20, editez_spread_box_pressed, 
									&editez_spread_value, -1.0f, -1.0f, 1.0f,
									 ez_textBufferSpread,&ez_spreadValueChanged,"Spread",40) )
	{
		editez_spread_box_pressed = !editez_spread_box_pressed;
	}
	
	okClicked = GuiButton( (Rectangle){ 400, 500, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 500, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
	if(okClicked)
	{
		name = std::string(ez_char_name);
		xPosition = xValueParam.current_value;
		yPosition = editez_y_value;
		zPosition = editez_z_value;
		width = editez_width_value;
		
		properties.flDamping = editez_damping_value;
		properties.flDelay = editez_delay_value;
		properties.flFeedback = editez_feedback_value;
		properties.flLRDelay = editez_lrdelay_value;
		properties.flSpread = editez_spread_value;
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
	
	memset(ez_char_name, 0, sizeof(ez_char_name));
	strncpy(ez_char_name, "name here", 20);
	ez_char_name[19] = '\0';
}
