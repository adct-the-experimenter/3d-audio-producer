#include "HRTF-Test-Dialog.h"

#include "raygui/raygui.h"

HRTFTestDialog::HRTFTestDialog()
{
	okClicked = false;
}

void HRTFTestDialog::SetPointerToAudioEngine(OpenAlSoftAudioEngine* audioEngine){ptrAudioEngine = audioEngine;}

std::string testResult = "";
char* textToDisplay = nullptr;

void HRTFTestDialog::InitGUI()
{
	//Test HRTF and put test results in textBox
	ptrAudioEngine->TestHRTF();
	
	testResult = ptrAudioEngine->getHRTFTestResult();
	
	ptrAudioEngine->clear_testHRTFResults();
	textToDisplay = const_cast<char*>(testResult.c_str());
	
}

void HRTFTestDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){300,100,400,500},"HRTF Test");
	
	//draw text box
	
	GuiTextBoxMulti((Rectangle){350,200,300,300}, textToDisplay, 12, false);
	
	//draw ok button
	okClicked = GuiButton( (Rectangle){ 450, 500, 70, 30 }, GuiIconText(0, "OK") );
	if(exit){okClicked = true;}
	
}

bool HRTFTestDialog::OkClickedOn(){return okClicked;}

void HRTFTestDialog::resetConfig()
{
	testResult.clear();
	okClicked = false;
}
