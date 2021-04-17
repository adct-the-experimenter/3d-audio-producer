#include "CreateSoundProducerDialog.h"


#include "raygui/raygui.h"

#include "raygui/raygui_extras.h"

#include <cstring>

CreateSoundProducerDialog::CreateSoundProducerDialog(const std::string& title)
{
	xPosition = 0.0; yPosition = 0.0; zPosition = 0.0;
	okClicked = false; cancelClicked = false;
	tempFreeRoamBool = false;
	m_sound_bank_ptr = nullptr;
	sound_choices = "";
}


void CreateSoundProducerDialog::SetPointerToSoundBank(SoundBank* thisSoundBank){m_sound_bank_ptr = thisSoundBank;}

static ValidFloatParamSettings xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

static ValidFloatParamSettings yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

static ValidFloatParamSettings zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

static std::uint8_t account_num;

static char char_name[20] = "name here";
static bool name_box_pressed = false;

static bool free_roam_box_stat = false;

static bool dropDownSoundMode = false;
static int dropDownSoundActive = 0;


void CreateSoundProducerDialog::DrawDialog()
{
	
	bool exit = GuiWindowBox((Rectangle){300,100,400,500},"Create Sound Producer");
	
	if(exit){cancelClicked = true;}
	
	
	if( GuiTextBox((Rectangle){400,200,100,50}, char_name, 20, name_box_pressed) )
	{
		name_box_pressed = !name_box_pressed;
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
	
	free_roam_box_stat = GuiCheckBox((Rectangle){ 400, 460, 20, 20 }, "Free Roam:", free_roam_box_stat);
	
	if(m_sound_bank_ptr)
	{
		if( GuiDropdownBox((Rectangle){ 400,400,140,30 }, sound_choices.c_str(), &dropDownSoundActive, dropDownSoundMode) )
		{
			dropDownSoundMode = !dropDownSoundMode;
		}
	}
	
	okClicked = GuiButton( (Rectangle){ 400, 500, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 500, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
	if(okClicked)
	{
		name = std::string(char_name);
		xPosition = xValueParam.current_value;
		yPosition = yValueParam.current_value;
		zPosition = zValueParam.current_value;
		tempFreeRoamBool = free_roam_box_stat;
		sound_bank_account_num = account_num;
	}
}

std::string CreateSoundProducerDialog::getNewName(){return name;}

void CreateSoundProducerDialog::getNewPosition(float& x, float& y, float& z)
{
	x = xPosition;
	y = yPosition;
	z = zPosition;
}


bool CreateSoundProducerDialog::OkClickedOn(){ return okClicked;}

bool CreateSoundProducerDialog::CancelClickedOn(){ return cancelClicked;}

bool CreateSoundProducerDialog::getFreeRoamBool(){return tempFreeRoamBool;}

std::uint8_t CreateSoundProducerDialog::getAccountNumber(){return sound_bank_account_num;}

void CreateSoundProducerDialog::resetConfig()
{
	xPosition = 0;
	yPosition = 0;
	zPosition = 0;
	
	xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

	yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

	zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");
	
	sound_bank_account_num = 0;
	okClicked = false;
	cancelClicked = false;
	tempFreeRoamBool = false;
	
	memset(char_name, 0, sizeof(char_name));
	strncpy(char_name, "name here", 20);
	char_name[19] = '\0';
}

void CreateSoundProducerDialog::InitSoundBankChoices()
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
}
