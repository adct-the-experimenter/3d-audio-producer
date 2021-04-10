#include "Change-HRTF-Dialog.h"


#include "raygui/raygui.h"
#include <cstring>

ChangeHRTFDialog::ChangeHRTFDialog()
{
	hrtf_selection_index = 0;
	okClicked = false;
}

void ChangeHRTFDialog::SetPointerToAudioEngine(OpenAlSoftAudioEngine* audioEngine){ptrAudioEngine = audioEngine;}


bool changehrtf_dropDownHRTFMode = false;
int changehrtf_dropDownHRTFActive = 0;
std::string hrtf_choices = "";
char log_message[100] = "";

void ChangeHRTFDialog::InitGUI()
{
	//add all HRTF names to listbox	
	ptrAudioEngine->GetAvailableHRTFNames(&hrtf_names);
	
	//initialize choices string
	for(size_t i = 0; i < hrtf_names.size(); i++)
	{
		hrtf_choices += hrtf_names.at(i) + ";";
	}
	
}

bool ChangeHRTFDialog::OkClickedOn(){return okClicked;}

void ChangeHRTFDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){300,100,400,500},"Change HRTF");
	
	
	GuiTextBox((Rectangle){400,200,200,200}, log_message, 100, false);
	
	okClicked = GuiButton( (Rectangle){ 400, 500, 70, 30 }, GuiIconText(0, "OK") );
	
	if( GuiButton( (Rectangle){ 400, 400, 70, 30 }, GuiIconText(0, "Change") ) )
	{
		std::string message;
		ptrAudioEngine->SelectThisHRTFByIndex(hrtf_selection_index,message);
		
		strncpy(log_message, message.c_str(), 100);
		log_message[99] = '\0';
		
	}
	
	if( GuiDropdownBox((Rectangle){ 400,150,140,30 }, hrtf_choices.c_str(), &changehrtf_dropDownHRTFActive, changehrtf_dropDownHRTFMode) )
	{
		changehrtf_dropDownHRTFMode = !changehrtf_dropDownHRTFMode;
		hrtf_selection_index = changehrtf_dropDownHRTFActive;
	}
	
}

void ChangeHRTFDialog::resetConfig()
{
	hrtf_names.clear();
	hrtf_choices.clear();
	okClicked = false;
	memset(log_message, 0, sizeof(log_message));
}
