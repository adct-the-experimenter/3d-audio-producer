#ifndef _CREATE_SOUND_PRODUCER_H
#define _CREATE_SOUND_PRODUCER_H

#include "openalsoftaudioengine.h" //for loading buffer and creating source of sound producer

#include "sound_bank.h"

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
	
	void SetPointerToSoundBank(SoundBank* thisSoundBank);
	
	//function to put choices for sound dropdown box
	void InitSoundBankChoices();

private:

	//sound producer attributes
	std::string name;
	double xPosition;
	double yPosition;
	double zPosition;
	bool tempFreeRoamBool;
	std::uint8_t sound_bank_account_num;
	
	//gui control
	bool okClicked;
	bool cancelClicked;
	
	SoundBank* m_sound_bank_ptr;
	std::string sound_choices;
};



#endif
