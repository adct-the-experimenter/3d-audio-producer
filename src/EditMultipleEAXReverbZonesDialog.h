#ifndef EDIT_MULTIPLE_EAX_REVERB_ZONES_DIALOG_H
#define EDIT_MULTIPLE_EAX_REVERB_ZONES_DIALOG_H

#include "effects-manager.h"

class EditMultipleEAXReverbZonesDialog
{

public:
	EditMultipleEAXReverbZonesDialog();


	void SetPointerToEffectsManager(EffectsManager* effects_manager);
	
	void InitGUI();
	
	void DrawDialog();

	void Preview();

	void resetConfig();
	
	bool OkClickedOn();
	bool CancelClickedOn();
	
private:

	EffectsManager* m_effects_manager_ptr; //pointer to vector of sound producers to edit
	
	ReverbEAXProperties tempEAXReverbProp;
		

	int m_selection_index;
	int spt_selection_index;
	

	void ChangeEAXReverbZoneAttributes();

	void ReverbZoneSelectedInListBox(size_t choice);
	
	
	void SoundProducerTrackSelectedInListBox();
	
	bool okClicked;
	bool cancelClicked;

};
#endif
