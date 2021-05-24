#include "EditMultipleEchoZonesDialog.h"

#define GUI_DROPDOWN_LISTVIEW

#include "raygui/raygui_extras.h"

#include <string>

EditMultipleEchoZonesDialog::EditMultipleEchoZonesDialog()
{
	okClicked = false;
	cancelClicked = false;
	
	m_selection_index = 0;
	spt_selection_index = 0;
}

void EditMultipleEchoZonesDialog::SetCurrentZoneIndexForEditing(size_t index){m_selection_index = index;}

void EditMultipleEchoZonesDialog::SetPointerToEffectsManager(EffectsManager* effects_manager){m_effects_manager_ptr = effects_manager;}

bool EditMultipleEchoZonesDialog::OkClickedOn(){return okClicked;}

bool EditMultipleEchoZonesDialog::CancelClickedOn(){return cancelClicked;}

static std::string echo_zone_choices = "";

//DropDownListViewSettings InitDropDownListViewSettings(int itemsShowCount, bool editMode, bool valueChanged, int scrollIndex)
static DropDownListViewSettings dropdown_listview_settings = InitDropDownListViewSettings(3,false,false,0);

static int editez_listview_activeIndex = 0;
static int editez_listview_itemsCount = 0;

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

void EditMultipleEchoZonesDialog::InitGUI()
{
	if(m_effects_manager_ptr)
	{
		//clear previous sound choices
		echo_zone_choices.clear();
				
		for(size_t i = 0; i < m_effects_manager_ptr->echo_zones_vector.size(); i++ )
		{
			echo_zone_choices += m_effects_manager_ptr->echo_zones_vector[i].GetNameString() + ";";
		}
		
		editez_listview_itemsCount = int( m_effects_manager_ptr->echo_zones_vector.size() );
		
		//initialize values in GUI text boxes based on first choice
		//EditMultipleSoundProducersDialog::SoundProducerSelectedInListBox(current_sound_producer_editing_index);
		EditMultipleEchoZonesDialog::EchoZoneSelectedInListBox(m_selection_index);
		
	}
}

void EditMultipleEchoZonesDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){300,100,400,500},"Edit Echo Zone");
	
	if(exit){cancelClicked = true;}
	
	
	//initialize text fields
	
	if( GuiTextBox((Rectangle){350,180,100,50}, ez_char_name, 20, ez_name_box_pressed) )
	{
		ez_name_box_pressed = !ez_name_box_pressed;
	}
    
    
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){350,250,50,50}, 20, &xValueParam, "X:", 10) )				
	{
		xValueParam.editMode = !xValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){450,250,50,50}, 20, &yValueParam,"Y:",10) )
	{
		yValueParam.editMode = !yValueParam.editMode;
	}
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){550,250,50,50}, 20, &zValueParam,"Z:",10) )
	{
		zValueParam.editMode = !zValueParam.editMode;
	}
    
    if( GuiTextBox_ValidValueFloatSimple((Rectangle){350,320,50,50}, 20, &widthValueParam,"Width:",40) )
	{
		widthValueParam.editMode = !widthValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){350,390,50,50}, 20, &delayValueParam,"Delay:",40) )
	{
		delayValueParam.editMode = !delayValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){455,390,50,50}, 20, &lrDelayValueParam,"LRDelay:",45) )
	{
		lrDelayValueParam.editMode = !lrDelayValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){350,460,50,50}, 20, &dampingValueParam,"Damping:",45) )
	{
		dampingValueParam.editMode = !dampingValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){455,460,50,50}, 20, &feedbackValueParam,"Feedback",50) )
	{
		feedbackValueParam.editMode = !feedbackValueParam.editMode;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){550,460,50,50}, 20, &spreadValueParam,"Spread",40) )
	{
		spreadValueParam.editMode = !spreadValueParam.editMode;
	}
	
	editez_listview_activeIndex = Gui_Dropdown_ListView_Simple(&dropdown_listview_settings, (Rectangle){ 350,130,140,40 }, 
																echo_zone_choices.c_str(), editez_listview_itemsCount,
																editez_listview_activeIndex
															);
											
	if(dropdown_listview_settings.valueChanged )
	{
		m_selection_index = editez_listview_activeIndex;
		EditMultipleEchoZonesDialog::EchoZoneSelectedInListBox(m_selection_index);
		dropdown_listview_settings.valueChanged = false;
	}
	
	okClicked = GuiButton( (Rectangle){ 400, 550, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 550, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
	if(okClicked)
	{
		EditMultipleEchoZonesDialog::ChangeEchoZoneAttributes();
	}
	
	/*
	
    wxStaticText* spPreviewText = new wxStaticText(this, -1, wxT("Echo Zone, Sound Track to Preview:"), wxPoint(40, 20));
    
    //make horizontal box to put names in
	wxBoxSizer* hboxEchoZones = new wxBoxSizer(wxHORIZONTAL);
	
	//list box to contain names of Sound Producers to edit, single selection by default 
	listboxEchoZones = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(100, 20)); 
	
	listboxEchoZones->Bind(wxEVT_LISTBOX,&EditMultipleEchoZonesDialog::EchoZoneSelectedInListBox,this);
	
	//add contents of reverb zones to listbox
	for(size_t i = 0; i < effects_manager_ptr->echo_zones_vector.size(); i++)
	{
		EchoZone* thisEchoZone = effects_manager_ptr->GetPointerToEchoZone(i);
					
		wxString mystring( thisEchoZone->GetNameString() );
		listboxEchoZones->Append(mystring);
	}
	
	//list box to contain names of Sound Producers to edit, single selection by default 
	listboxSoundProducers = new wxListBox(this, wxID_ANY,wxPoint(0, 0), wxSize(100, 20)); 
	
	listboxSoundProducers->Bind(wxEVT_LISTBOX,&EditMultipleEchoZonesDialog::SoundProducerTrackSelectedInListBox,this);
	
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


void EditMultipleEchoZonesDialog::ChangeEchoZoneAttributes()
{
	
	if(m_selection_index != -1)
	{
		if(m_effects_manager_ptr->echo_zones_vector.size() > 0)
		{
			
			EchoZone* thisEchoZone = &m_effects_manager_ptr->echo_zones_vector.at(m_selection_index);
			
			std::string name = std::string(ez_char_name);
			thisEchoZone->SetNameString(name);	
			
			thisEchoZone->SetPositionX(xValueParam.current_value);
			thisEchoZone->SetPositionY(yValueParam.current_value);
			thisEchoZone->SetPositionZ(zValueParam.current_value);
			
			thisEchoZone->ChangeWidth(widthValueParam.current_value);
			
			tempEchoProp.flDelay = delayValueParam.current_value;
			tempEchoProp.flDamping = dampingValueParam.current_value;
			tempEchoProp.flFeedback = feedbackValueParam.current_value;
			tempEchoProp.flLRDelay = lrDelayValueParam.current_value;
			tempEchoProp.flSpread = spreadValueParam.current_value;
			
			thisEchoZone->ChangeEchoZoneProperties(tempEchoProp);
			
		}
			
	}
	
	
}
	


void EditMultipleEchoZonesDialog::Preview()
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


void EditMultipleEchoZonesDialog::EchoZoneSelectedInListBox(size_t choice)
{
	
	if(m_effects_manager_ptr->echo_zones_vector.size() > 0)
	{
		EchoZone* thisEchoZone = &m_effects_manager_ptr->echo_zones_vector.at(choice);
		
		strncpy(ez_char_name, thisEchoZone->GetNameString().c_str(), 20);
		ez_char_name[19] = '\0';
		
		xValueParam = InitValidFloatParamSettings(thisEchoZone->GetPositionX(), 0.0f, -30.0f, 30.0f, std::to_string(thisEchoZone->GetPositionX()).c_str() );

		yValueParam = InitValidFloatParamSettings(thisEchoZone->GetPositionY(), 0.0f, -30.0f, 30.0f, std::to_string(thisEchoZone->GetPositionY()).c_str() );

		zValueParam = InitValidFloatParamSettings(thisEchoZone->GetPositionZ(), 0.0f, -30.0f, 30.0f, std::to_string(thisEchoZone->GetPositionZ()).c_str() );

		widthValueParam = InitValidFloatParamSettings(thisEchoZone->GetWidth(), 10.0f, 1.0f, 40.0f, std::to_string(thisEchoZone->GetWidth()).c_str() );

		tempEchoProp = thisEchoZone->GetEchoZoneProperties();
		
		delayValueParam = InitValidFloatParamSettings(tempEchoProp.flDelay, 0.1f, 0.0f, 0.207f, std::to_string(tempEchoProp.flDelay).c_str() );

		lrDelayValueParam = InitValidFloatParamSettings(tempEchoProp.flLRDelay, 0.0f, 0.0f, 0.404f, std::to_string(tempEchoProp.flLRDelay).c_str() );

		dampingValueParam = InitValidFloatParamSettings(tempEchoProp.flDamping, 0.5f, 0.0f, 0.99f, std::to_string(tempEchoProp.flDamping).c_str() );

		feedbackValueParam = InitValidFloatParamSettings(tempEchoProp.flFeedback, 0.5f, 0.0f, 1.0f, std::to_string(tempEchoProp.flFeedback).c_str() );

		spreadValueParam = InitValidFloatParamSettings(tempEchoProp.flSpread, -1.0f, -1.0f, 1.0f, std::to_string(tempEchoProp.flSpread).c_str() );
		
	}
	
}

void EditMultipleEchoZonesDialog::SoundProducerTrackSelectedInListBox()
{
	//spt_selection_index = listboxSoundProducers->GetSelection();
}

void EditMultipleEchoZonesDialog::resetConfig()
{
	okClicked = false;
	cancelClicked = false;
}
