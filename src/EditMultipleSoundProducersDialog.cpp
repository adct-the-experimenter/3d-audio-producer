#include "EditMultipleSoundProducersDialog.h"


#include "raygui/raygui.h"

#include <cstring>

EditMultipleSoundProducersDialog::EditMultipleSoundProducersDialog(const std::string& title
																	)
{
	current_sound_producer_editing_index = 0;
}

void EditMultipleSoundProducersDialog::Init(std::vector <std::unique_ptr <SoundProducer>> *sound_producer_vector){sound_producer_vector_ref = sound_producer_vector;}

void EditMultipleSoundProducersDialog::SetPointerToSoundBank(SoundBank* thisSoundBank){m_sound_bank_ptr = thisSoundBank;}


int editsp_x_value = 0;
bool editsp_x_box_pressed = false;
int editsp_y_value = 0;
bool editsp_y_box_pressed = false;
int editsp_z_value = 0;
bool editsp_z_box_pressed = false;

char editsp_char_name[20];
bool editsp_name_box_pressed = false;

bool editsp_free_roam_box_stat = false;

bool editsp_dropDownSoundMode = false;
int editsp_dropDownSoundActive = 0;

bool editsp_dropDownSoundProducerMode = false;
int editsp_dropDownSoundProducerActive = 0;
std::string soundproducer_choices = "";

void EditMultipleSoundProducersDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){300,100,400,500},"Edit Sound Producer");
	
	if(exit){cancelClicked = true;}
	
	
	if(sound_producer_vector_ref)
	{
		if(soundproducer_choices == "")
		{
			for(size_t i = 0; i < sound_producer_vector_ref->size(); i++)
			{
				soundproducer_choices += sound_producer_vector_ref->at(i)->GetNameString() + ";";
			}
		}
		
		if( GuiDropdownBox((Rectangle){ 400,150,140,30 }, soundproducer_choices.c_str(), &editsp_dropDownSoundProducerActive, editsp_dropDownSoundProducerMode) )
		{
			editsp_dropDownSoundProducerMode = !editsp_dropDownSoundProducerMode;
			current_sound_producer_editing_index = (size_t)editsp_dropDownSoundProducerActive;
			EditMultipleSoundProducersDialog::SoundProducerSelectedInListBox(current_sound_producer_editing_index);
		}
	}
	
	if( GuiTextBox((Rectangle){400,200,100,50}, editsp_char_name, 20, editsp_name_box_pressed) )
	{
		editsp_name_box_pressed = !editsp_name_box_pressed;
	}
	
	if( GuiValueBox((Rectangle){400,300,50,50}, "X:", &editsp_x_value, -10, 10, editsp_x_box_pressed) )
	{
		editsp_x_box_pressed = !editsp_x_box_pressed;
	}
	if( GuiValueBox((Rectangle){500,300,50,50}, "Y:", &editsp_y_value, -10, 10, editsp_y_box_pressed) )
	{
		editsp_y_box_pressed = !editsp_y_box_pressed;
	}
	if( GuiValueBox((Rectangle){600,300,50,50}, "Z:", &editsp_z_value, -10, 10, editsp_z_box_pressed) )
	{
		editsp_z_box_pressed = !editsp_z_box_pressed;
	}
	
	tempFreeRoamBool = GuiCheckBox((Rectangle){ 400, 460, 20, 20 }, "Free Roam:", tempFreeRoamBool);
	
	if(m_sound_bank_ptr)
	{
		if( GuiDropdownBox((Rectangle){ 400,400,140,30 }, sound_choices.c_str(), &editsp_dropDownSoundActive, editsp_dropDownSoundMode) )
		{
			editsp_dropDownSoundMode = !editsp_dropDownSoundMode;
			sound_bank_account_num = editsp_dropDownSoundActive;
		}
	}
	
	okClicked = GuiButton( (Rectangle){ 400, 500, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 500, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
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
			xPosition = (double)editsp_x_value;
			yPosition = (double)editsp_y_value;
			zPosition = (double)editsp_z_value;
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
	
	editsp_x_value = 0;
	editsp_y_value = 0;
	editsp_z_value = 0;
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
			
			editsp_x_value = int(xPosition);
			editsp_y_value = int(yPosition);
			editsp_z_value = int(zPosition);
			
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
		
		//initialize values in GUI text boxes based on first choice
		EditMultipleSoundProducersDialog::SoundProducerSelectedInListBox(0);
		
	}
}
