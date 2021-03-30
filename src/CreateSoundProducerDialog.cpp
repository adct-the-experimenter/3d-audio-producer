#include "CreateSoundProducerDialog.h"

#undef RAYGUI_IMPLEMENTATION

#include "raygui/raygui.h"


CreateSoundProducerDialog::CreateSoundProducerDialog(const std::string& title)
{
	xPosition = 0.0; yPosition = 0.0; zPosition = 0.0;
	okClicked = false; cancelClicked = false;
	tempFreeRoamBool = false;
}

int x_value = 0;
bool x_box_pressed = false;
int y_value = 0;
bool y_box_pressed = false;
int z_value = 0;
bool z_box_pressed = false;

char char_name[20] = "name here";
bool name_box_pressed = false;

bool free_roam_box_stat = false;

void CreateSoundProducerDialog::DrawDialog()
{
	
	
	bool exit = GuiWindowBox((Rectangle){300,100,400,400},"Create Sound Producer");
	
	if(exit){cancelClicked = true;}
	
	
	if( GuiTextBox((Rectangle){400,200,100,50}, char_name, 20, name_box_pressed) )
	{
		name_box_pressed = !name_box_pressed;
	}
	
	if( GuiValueBox((Rectangle){400,300,50,50}, "X:", &x_value, -10, 10, x_box_pressed) )
	{
		x_box_pressed = !x_box_pressed;
	}
	if( GuiValueBox((Rectangle){500,300,50,50}, "Y:", &y_value, -10, 10, y_box_pressed) )
	{
		y_box_pressed = !y_box_pressed;
	}
	if( GuiValueBox((Rectangle){600,300,50,50}, "Z:", &z_value, -10, 10, z_box_pressed) )
	{
		z_box_pressed = !z_box_pressed;
	}
	
	free_roam_box_stat = GuiCheckBox((Rectangle){ 400, 400, 20, 20 }, "Free Roam:", free_roam_box_stat);
	
	okClicked = GuiButton( (Rectangle){ 400, 450, 70, 30 }, GuiIconText(0, "OK") );
	
	cancelClicked = GuiButton( (Rectangle){ 500, 450, 70, 30 }, GuiIconText(0, "Cancel") );
	if(exit){cancelClicked = true;}
	
	if(okClicked)
	{
		name = std::string(char_name);
		xPosition = (double)x_value;
		yPosition = (double)y_value;
		zPosition = (double)z_value;
		tempFreeRoamBool = free_roam_box_stat;
	}
}

std::string CreateSoundProducerDialog::getNewName(){return name;}

void CreateSoundProducerDialog::getNewPosition(double& x, double& y, double& z)
{
	x = xPosition;
	y = yPosition;
	z = zPosition;
}


bool CreateSoundProducerDialog::OkClickedOn(){ return okClicked;}

bool CreateSoundProducerDialog::CancelClickedOn(){ return cancelClicked;}

bool CreateSoundProducerDialog::getFreeRoamBool(){return tempFreeRoamBool;}

void CreateSoundProducerDialog::resetConfig()
{
	xPosition = 0;
	yPosition = 0;
	zPosition = 0;
	
	x_value = 0;
	y_value = 0;
	z_value = 0;
	okClicked = false;
	cancelClicked = false;
	tempFreeRoamBool = false;
}
