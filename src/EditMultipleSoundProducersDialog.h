#ifndef _EDIT_MULTIPLE_SOUND_PRODUCERS_H
#define _EDIT_MULTIPLE_SOUND_PRODUCERS_H

#include <memory>

#include "sound_bank.h"
#include "soundproducer.h"

#include "openalsoftaudioengine.h" //for loading buffer and creating source of sound producer

#include <string>

class EditMultipleSoundProducersDialog
{

public:
	EditMultipleSoundProducersDialog(const std::string& title);
	
	void Init(std::vector <std::unique_ptr <SoundProducer>> *sound_producer_vector);
	
	void DrawDialog();

	bool OkClickedOn();
	bool CancelClickedOn();
	
	void SetPointerToSoundBank(SoundBank* thisSoundBank);
	
	//function to initialize text boxes such as sound bank 
	void InitGUI();
	
	void SetCurrentSoundProducerEditedIndex(size_t index);
	
	void resetConfig();
	
private:

	std::vector <std::unique_ptr <SoundProducer> > *sound_producer_vector_ref; //pointer to vector of sound producers to edit
	
	bool okClicked; //bool to indicate if ok button was clicked on
	bool cancelClicked;
	
	void SoundProducerSelectedInListBox(size_t choice);
	void ChangeSoundProducerAttributes();
	
	//sound producer attributes
	std::string name;
	double xPosition;
	double yPosition;
	double zPosition;
	bool tempFreeRoamBool;
	std::uint8_t sound_bank_account_num;

	//index of the current sound producer being edited
	size_t current_sound_producer_editing_index;
	
	SoundBank* m_sound_bank_ptr;
	std::string sound_choices;

};



#endif
