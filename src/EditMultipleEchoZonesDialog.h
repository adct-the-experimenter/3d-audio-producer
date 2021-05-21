#ifndef EDIT_MULTIPLE_ECHO_ZONES_DIALOG_H
#define EDIT_MULTIPLE_ECHO_ZONES_DIALOG_H

#include "effects-manager.h"

class EditMultipleEchoZonesDialog
{

public:
	EditMultipleEchoZonesDialog();

	void SetPointerToEffectsManager(EffectsManager* effects_manager);
	
	void InitGUI();
	
	void DrawDialog();

	void Preview();

	void resetConfig();
	
	bool OkClickedOn();
	bool CancelClickedOn();
	
	void SetCurrentZoneIndexForEditing(size_t index);
	
private:

	EffectsManager* m_effects_manager_ptr; //pointer to vector of sound producers to edit
	
	EchoZoneProperties tempEchoProp;
		

	int m_selection_index;
	int spt_selection_index;
	

	void ChangeEchoZoneAttributes();

	void EchoZoneSelectedInListBox(size_t choice);
	
	
	void SoundProducerTrackSelectedInListBox();
	
	bool okClicked;
	bool cancelClicked;

};

#endif
