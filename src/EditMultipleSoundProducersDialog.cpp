#include "EditMultipleSoundProducersDialog.h"

#include "global_variables.h"

#include "raygui/raygui.h"

#include <cstring>
#include <string>

#include "raygui/gui_valid_float.h"
#include "raygui/gui_dropdown_listview.h"

EditMultipleSoundProducersDialog::EditMultipleSoundProducersDialog(const std::string& title)
{
	current_sound_producer_editing_index = 0;
}

void EditMultipleSoundProducersDialog::Init(std::vector <std::unique_ptr <SoundProducer>> *sound_producer_vector){sound_producer_vector_ref = sound_producer_vector;}

void EditMultipleSoundProducersDialog::SetPointerToSoundBank(SoundBank* thisSoundBank){m_sound_bank_ptr = thisSoundBank;}

//DropDownListViewSettings InitDropDownListViewSettings(int itemsShowCount, bool editMode, bool valueChanged, int scrollIndex)
static DropDownListViewSettings soundproducer_dropdown_listview_settings = InitDropDownListViewSettings(3,false,false,0);

static int edit_soundproducer_listview_activeIndex = 0;
static int edit_soundproducer_listview_itemsCount = 0;


static ValidFloatParamSettings xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

static ValidFloatParamSettings yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

static ValidFloatParamSettings zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");

static char editsp_char_name[20] = "name here";
static bool editsp_name_box_pressed = false;

static bool editsp_free_roam_box_stat = false;

static bool editsp_dropDownSoundMode = false;
static int editsp_dropDownSoundActive = 0;

static bool editsp_dropDownSoundProducerMode = false;
static int editsp_dropDownSoundProducerActive = 0;
static std::string soundproducer_choices = "";

void EditMultipleSoundProducersDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){300,100,400,500},"Edit Sound Producer");
	
	if(exit){cancelClicked = true;}
	
	okClicked = GuiButton( (Rectangle){ 400, 500, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 500, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
	
	if( GuiTextBox((Rectangle){400,200,100,50}, editsp_char_name, 20, editsp_name_box_pressed) )
	{
		editsp_name_box_pressed = !editsp_name_box_pressed;
	}
	
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){400,300,50,50}, 20, &xValueParam,"X:",10) )
	{
		xValueParam.editMode = !xValueParam.editMode;
	}
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){500,300,50,50}, 20, &yValueParam,"Y:",10) )
	{
		yValueParam.editMode = !yValueParam.editMode;
	}
	if( GuiTextBox_ValidValueFloatSimple((Rectangle){600,300,50,50}, 20, &zValueParam,"Z:",10) )
	{
		zValueParam.editMode = !zValueParam.editMode;
	}

	
	tempFreeRoamBool = GuiCheckBox((Rectangle){ 400, 460, 20, 20 }, "Free Roam:", tempFreeRoamBool);
	
	if(m_sound_bank_ptr)
	{
		
		if( GuiDropdownBox((Rectangle){ 400,380,140,30 }, sound_choices.c_str(), &editsp_dropDownSoundActive, editsp_dropDownSoundMode) )
		{
			editsp_dropDownSoundMode = !editsp_dropDownSoundMode;
			sound_bank_account_num = editsp_dropDownSoundActive;
		}
		
		
	}
	
	
	
	edit_soundproducer_listview_activeIndex = Gui_Dropdown_ListView_Simple(&soundproducer_dropdown_listview_settings, (Rectangle){ 400,150,140,40 }, 
																soundproducer_choices.c_str(), edit_soundproducer_listview_itemsCount,
																edit_soundproducer_listview_activeIndex
															);
											
	if(soundproducer_dropdown_listview_settings.valueChanged )
	{
		current_sound_producer_editing_index = (size_t)edit_soundproducer_listview_activeIndex;
		EditMultipleSoundProducersDialog::SoundProducerSelectedInListBox(current_sound_producer_editing_index);
		soundproducer_dropdown_listview_settings.valueChanged = false;
	}
	
	
	if(okClicked)
	{
		EditMultipleSoundProducersDialog::ChangeSoundProducerAttributes();
	}
}

void EditMultipleSoundProducersDialog::ChangeSoundProducerAttributes()
{
	
	if(sound_producer_vector_ref != nullptr)
	{
		if(sound_producer_vector_ref->size() > 0)
		{
			SoundProducer* thisSoundProducer = sound_producer_vector_ref->at(current_sound_producer_editing_index).get();
			
			std::string newname = std::string(editsp_char_name);
			thisSoundProducer->SetNameString(newname);
			
			//change position of selected sound producer based on what is in textfields
			xPosition = xValueParam.current_value;
			yPosition = yValueParam.current_value;
			zPosition = zValueParam.current_value;
			thisSoundProducer->SetPositionX(xPosition);
			thisSoundProducer->SetPositionY(yPosition);
			thisSoundProducer->SetPositionZ(zPosition);
			
			thisSoundProducer->SetFreeRoamBool(tempFreeRoamBool);
			thisSoundProducer->SetAccountNumber(sound_bank_account_num);
		}
		
	}
	
}


void EditMultipleSoundProducersDialog::resetConfig()
{
	current_sound_producer_editing_index = 0;
	
	xPosition = 0;
	yPosition = 0;
	zPosition = 0;
	
	xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");
	yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");
	zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, min_position_value, max_position_value, "0.0");
	
	sound_bank_account_num = 0;
	okClicked = false;
	cancelClicked = false;
	tempFreeRoamBool = false;
	soundproducer_choices = "";
	memset(editsp_char_name, 0, sizeof(editsp_char_name));
}

void EditMultipleSoundProducersDialog::SoundProducerSelectedInListBox(size_t choice)
{
	current_sound_producer_editing_index = choice;
	
	if(sound_producer_vector_ref != nullptr)
	{
		if(sound_producer_vector_ref->size() > 0)
		{
			SoundProducer* thisSoundProducer = sound_producer_vector_ref->at(choice).get();
		
			tempFreeRoamBool = thisSoundProducer->GetFreeRoamBool();
			
			strncpy(editsp_char_name, thisSoundProducer->GetNameString().c_str(), 20);
			editsp_char_name[19] = '\0';
			
			//update position text fields to have current position of sound producer selected
			xPosition = thisSoundProducer->GetPositionX();
			yPosition = thisSoundProducer->GetPositionY();
			zPosition = thisSoundProducer->GetPositionZ();
			
			xValueParam = InitValidFloatParamSettings(xPosition, 0.0f, min_position_value, max_position_value, std::to_string(xPosition).c_str());
			yValueParam = InitValidFloatParamSettings(yPosition, 0.0f, min_position_value, max_position_value, std::to_string(yPosition).c_str());
			zValueParam = InitValidFloatParamSettings(zPosition, 0.0f, min_position_value, max_position_value, std::to_string(zPosition).c_str());
			
			//update sound bank account number
			sound_bank_account_num = thisSoundProducer->GetAccountNumber();
			editsp_dropDownSoundActive = thisSoundProducer->GetAccountNumber();
		}
		
	}
}

bool EditMultipleSoundProducersDialog::OkClickedOn(){ return okClicked;}

bool EditMultipleSoundProducersDialog::CancelClickedOn(){return cancelClicked;}

void EditMultipleSoundProducersDialog::InitGUI()
{
	
	if(m_sound_bank_ptr)
	{
		//clear previous sound choices
		sound_choices = "";
		
		//put choices into specific format
		std::array <std::string,10> account_look_up = m_sound_bank_ptr->GetAccountLookupTable();
		
		for(size_t i = 0; i < account_look_up.size(); i++ )
		{
			sound_choices += std::to_string(i) + " | " + account_look_up[i] + ";";
		}
				
		
		
	}
	
	if(sound_producer_vector_ref)
	{
		if(soundproducer_choices == "")
		{
			for(size_t i = 0; i < sound_producer_vector_ref->size(); i++)
			{
				soundproducer_choices += sound_producer_vector_ref->at(i)->GetNameString() + ";";
			}
			
			edit_soundproducer_listview_itemsCount = (int)sound_producer_vector_ref->size();
		}
				
	}
	
	//initialize values in GUI text boxes based on first choice
	EditMultipleSoundProducersDialog::SoundProducerSelectedInListBox(current_sound_producer_editing_index);
}

void EditMultipleSoundProducersDialog::SetCurrentSoundProducerEditedIndex(size_t index)
{
	current_sound_producer_editing_index = index;
	editsp_dropDownSoundProducerActive = current_sound_producer_editing_index;
	
}
