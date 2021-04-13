#include "EditListenerDialog.h"

#include "raygui/raygui.h"

#include "raygui/raygui_extras.h"

EditListenerDialog::EditListenerDialog(const std::string& title)
{
	
}


float editlt_x_value = 0;
bool editlt_x_box_pressed = false;
int editlt_y_value = 0;
bool editlt_y_box_pressed = false;
int editlt_z_value = 0;
bool editlt_z_box_pressed = false;

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
		editlt_x_value = ptrListener->getPositionX();
		editlt_y_value = int(ptrListener->getPositionY());
		editlt_z_value = int(ptrListener->getPositionZ());
		
		
		editlt_tempFreeRoamBool = ptrListener->GetListenerFreeRoamBool();
		
		editlt_extOrientationBool = ptrListener->GetListenerExternalDeviceOrientationBool();
	}
}


void EditListenerDialog::DrawDialog()
{
	bool exit = GuiWindowBox((Rectangle){300,100,400,500},"Edit Sound Producer");
	
	if(exit){cancelClicked = true;}
	
	
	//if( GuiValueBox((Rectangle){400,300,50,50}, "X:", &editlt_x_value, -10, 10, editlt_x_box_pressed) )
	//{
	//	editlt_x_box_pressed = !editlt_x_box_pressed;
	//}
	if( GuiTextBox_ValidValueFloat((Rectangle){400,300,50,50}, 20, editlt_x_box_pressed, 
									&editlt_x_value, 0.0f, -10.0f, 10.0f ) )
	{
		editlt_x_box_pressed = !editlt_x_box_pressed;
	}
	if( GuiValueBox((Rectangle){500,300,50,50}, "Y:", &editlt_y_value, -10, 10, editlt_y_box_pressed) )
	{
		editlt_y_box_pressed = !editlt_y_box_pressed;
	}
	if( GuiValueBox((Rectangle){600,300,50,50}, "Z:", &editlt_z_value, -10, 10, editlt_z_box_pressed) )
	{
		editlt_z_box_pressed = !editlt_z_box_pressed;
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
		float xPosition, yPosition, zPosition;
		xPosition = float(editlt_x_value);
		yPosition = float(editlt_x_value);
		zPosition = float(editlt_x_value);
		
		ptrListener->setPositionX(xPosition);
		ptrListener->setPositionY(yPosition);
		ptrListener->setPositionZ(zPosition);
		
		//change free roam status
		ptrListener->SetListenerFreeRoamBool(editlt_tempFreeRoamBool);
		
		//change external device orientation status
		ptrListener->SetListenerExternalDeviceOrientationBool(editlt_extOrientationBool);
		
	}
	
}

void EditListenerDialog::resetConfig()
{
	editlt_x_value = 0;
	editlt_x_box_pressed = false;
	editlt_y_value = 0;
	editlt_y_box_pressed = false;
	editlt_z_value = 0;
	editlt_z_box_pressed = false;

	editlt_tempFreeRoamBool = false;
	editlt_free_roam_box_stat = false;

	editlt_extOrientationBool = false;
	editlt_ext_orient_box_stat = false;
	
	okClicked = false;
	cancelClicked = false;
}

bool EditListenerDialog::OkClickedOn(){ return okClicked;}

bool EditListenerDialog::CancelClickedOn(){return cancelClicked;}
