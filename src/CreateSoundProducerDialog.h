#ifndef _CREATE_SOUND_PRODUCER_H
#define _CREATE_SOUND_PRODUCER_H


#include "openalsoftaudioengine.h" //for loading buffer and creating source of sound producer

class CreateSoundProducerDialog
{

public:
	CreateSoundProducerDialog(const std::string& title);

	void DrawDialog();

	//function to return position of new sound producer object
	void getNewPosition(double& x, double& y, double& z);

	std::string getNewName();

	bool OkClickedOn();
	bool CancelClickedOn();
	
	bool getFreeRoamBool();
	
	void resetConfig();

private:

	OpenAlSoftAudioEngine* ptrAudioEngine;

	std::string name;
	double xPosition;
	double yPosition;
	double zPosition;
	bool tempFreeRoamBool;
	
	bool okClicked;
	bool cancelClicked;
	
};



#endif
