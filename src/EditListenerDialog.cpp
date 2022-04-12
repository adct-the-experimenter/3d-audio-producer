#include "EditListenerDialog.h"

#include "raygui/raygui.h"

#define GUI_VALIDFLOATBOX
#include "raygui/gui_valid_float.h"

#include <string>

EditListenerDialog::EditListenerDialog(const std::string& title)
{
	
}


static ValidFloatParamSettings xValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

static ValidFloatParamSettings yValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");

static ValidFloatParamSettings zValueParam = InitValidFloatParamSettings(0.0f, 0.0f, -30.0f, 30.0f, "0.0");


bool editlt_tempFreeRoamBool = false;
bool editlt_free_roam_box_stat = false;

bool editlt_extOrientationBool = false;
bool editlt_ext_orient_box_stat = false;

void EditListenerDialog::SetPointerToListener(Listener* listener){ptrListener = listener;}

void EditListenerDialog::InitGUI()
{
	if(ptrListener == nullptr){std::cout << "listener pointer is nullptr! \n";}
	else
	{
		xValueParam = InitValidFloatParamSettings(ptrListener->getPositionX(), 0.0f, -30.0f, 30.0f,std::to_string(ptrListener->getPositionX()).c_str() );
		
		yValueParam = InitValidFloatParamSettings(ptrListener->getPositionY(), 0.0f, -30.0f, 30.0f,std::to_string(ptrListener->getPositionY()).c_str() );
		
		zValueParam = InitValidFloatParamSettings(ptrListener->getPositionZ(), 0.0f, -30.0f, 30.0f,std::to_string(ptrListener->getPositionZ()).c_str() );
		
		editlt_tempFreeRoamBool = ptrListener->GetListenerFreeRoamBool();
		
		editlt_extOrientationBool = ptrListener->GetListenerExternalDeviceOrientationBool();
	}
}



void EditListenerDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){300,100,400,500},"Edit Sound Producer");
	
	if(exit){cancelClicked = true;}
	
	
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
	
	editlt_tempFreeRoamBool = GuiCheckBox((Rectangle){ 400, 400, 20, 20 }, "Free Roam:", editlt_tempFreeRoamBool);
	
	editlt_extOrientationBool = GuiCheckBox((Rectangle){ 400, 450, 20, 20 }, "External Device Orientation:", editlt_extOrientationBool);
	
	
	okClicked = GuiButton( (Rectangle){ 400, 500, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 500, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
	if(okClicked)
	{
		EditListenerDialog::ChangeListenerAttributes();
	}
	
}


void EditListenerDialog::ChangeListenerAttributes()
{	
	if(ptrListener != nullptr)
	{
		//change position of selected sound producer based on what is in textfields
		
		ptrListener->setPositionX(xValueParam.current_value);
		ptrListener->setPositionY(yValueParam.current_value);
		ptrListener->setPositionZ(zValueParam.current_value);
		
		//change free roam status
		ptrListener->SetListenerFreeRoamBool(editlt_tempFreeRoamBool);
		
		//change external device orientation status
		ptrListener->SetListenerExternalDeviceOrientationBool(editlt_extOrientationBool);
		
	}
	
}

void EditListenerDialog::resetConfig()
{
	editlt_tempFreeRoamBool = false;
	editlt_free_roam_box_stat = false;

	editlt_extOrientationBool = false;
	editlt_ext_orient_box_stat = false;
	
	okClicked = false;
	cancelClicked = false;
}

bool EditListenerDialog::OkClickedOn(){ return okClicked;}

bool EditListenerDialog::CancelClickedOn(){return cancelClicked;}
