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

void CreateSoundProducerDialog::DrawDialog()
{
	//initialize text fields
	
	bool exit = GuiWindowBox((Rectangle){300,100,400,400},"Create Sound Producer");
	
	if(exit){cancelClicked = true;}
	
	GuiValueBox((Rectangle){400,100,100,50}, "X", &x_value, -10, 10, true);
	/*
	wxFloatingPointValidator <double> validator(2,nullptr,wxNUM_VAL_ZERO_AS_BLANK);
    validator.SetRange(-10.00,10.00);     // set allowable range
    
    textFieldName = new wxTextCtrl(this,-1, "Name", 
								wxPoint(95, 20), wxSize(80,20),
								wxTE_PROCESS_ENTER);
    
	textFieldX = new wxTextCtrl(this,-1, "0.00", 
								wxPoint(95, 60), wxSize(80,20),
								wxTE_PROCESS_ENTER,
								validator,          // associate the text box with the desired validator
								wxT(""));
								
	textFieldY = new wxTextCtrl(this,-1, "0.00", 
								wxPoint(95, 80), wxSize(80,20),
								wxTE_PROCESS_ENTER,
								validator,          // associate the text box with the desired validator
								wxT(""));
								
	textFieldZ = new wxTextCtrl(this,-1, "0.00", 
								wxPoint(95, 100), wxSize(80,20),
								wxTE_PROCESS_ENTER,
								validator,          // associate the text box with the desired validator
								wxT("")); 
	
	//initialize text to the left of fields
	wxStaticText* NameText = new wxStaticText(this, -1, wxT("Name :"), wxPoint(40, 20));
	wxStaticText* positionText = new wxStaticText(this, -1, wxT("Position :"), wxPoint(20, 40));
	wxStaticText* xPositionText = new wxStaticText(this, -1, wxT("X :"), wxPoint(40, 60));
	wxStaticText* yPositionText = new wxStaticText(this, -1, wxT("Y :"), wxPoint(40, 80));
	wxStaticText* zPositionText = new wxStaticText(this, -1, wxT("Z :"), wxPoint(40, 100)); 
    
    //initialize Ok and Cancel buttons 
	okButton = new wxButton(this, CreateSoundProducerDialog::ID_OK, wxT("Ok"), 
							wxDefaultPosition, wxSize(70, 30));

	cancelButton = new wxButton(this, CreateSoundProducerDialog::ID_CANCEL, wxT("Cancel"), 
								wxDefaultPosition, wxSize(70, 30));
								
	//add checkmark box to determine if sound producer can roam freely in world or is controlled by sound producer track
	checkBoxFreeRoam = new wxCheckBox(this, wxID_ANY, wxT("Free Roam"), wxDefaultPosition, wxSize(30,30));

	//Make vertical box to put horizontal boxes in
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	
	//make horizontal box to put ok and cancel buttons in
	wxBoxSizer *hbox5 = new wxBoxSizer(wxHORIZONTAL);

	hbox5->Add(okButton, 1);
	hbox5->Add(cancelButton, 1, wxLEFT, 5);
	
	//add panel of text fields in vertical box
	
	wxBoxSizer *hboxName = new wxBoxSizer(wxHORIZONTAL);
	hboxName->Add(NameText);
	hboxName->Add(textFieldName);
	
	vbox->Add(hboxName, 1, wxEXPAND | wxALL, 10);
	
	vbox->Add(positionText);
	
	wxBoxSizer *hboxX = new wxBoxSizer(wxHORIZONTAL);
	hboxX->Add(xPositionText);
	hboxX->Add(textFieldX);
	
	vbox->Add(hboxX,1, wxEXPAND | wxALL, 10);
	
	wxBoxSizer *hboxY = new wxBoxSizer(wxHORIZONTAL);
	hboxY->Add(yPositionText);
	hboxY->Add(textFieldY);
	
	vbox->Add(hboxY,1, wxEXPAND | wxALL, 10);
	
	wxBoxSizer *hboxZ = new wxBoxSizer(wxHORIZONTAL);
	hboxZ->Add(zPositionText);
	hboxZ->Add(textFieldZ);
	
	vbox->Add(hboxZ,1, wxEXPAND | wxALL, 10);
	
	vbox->Add(checkBoxFreeRoam,1, wxEXPAND | wxALL, 10);
	
	vbox->Add(hbox5, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 10);

	SetSizerAndFit(vbox);
	*/
	
	if(okClicked)
	{
		xPosition = (double)x_value;
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
	okClicked = false;
	cancelClicked = false;
}
