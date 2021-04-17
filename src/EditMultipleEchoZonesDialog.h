#ifndef EDIT_MULTIPLE_ECHO_ZONES_DIALOG_H
#define EDIT_MULTIPLE_ECHO_ZONES_DIALOG_H

#include <memory>
#include <wx/wx.h>

#include <wx/valnum.h> //for wxFloatingPointValidator
#include <wx/textctrl.h> //for wxTextCtrl
#include <wx/listbox.h> //for list box

#include "effects-manager.h"

class EditMultipleEchoZonesDialog
{

public:
	EditMultipleEchoZonesDialog();

	void SetPointerToEffectsManager(EffectsManager* effects_manager);

	void OnOk(wxCommandEvent& event );

	void OnCancel(wxCommandEvent& event);

	void OnApply(wxCommandEvent& event);

	void OnPreview(wxCommandEvent& event);

	void Exit();


private:

	EffectsManager* m_effects_manager_ptr; //pointer to vector of sound producers to edit
	
	EchoZoneProperties tempEchoProp;
	
	std::string name;
	double xPosition;
	double yPosition;
	double zPosition;
	double width;

	int m_selection_index;
	
	void initPrivateVariables();

	void ChangeEchoZoneAttributes();

	void EchoZoneSelectedInListBox(wxCommandEvent& event );
	
	wxListBox* listboxSoundProducers;
	int spt_selection_index;
	
	void SoundProducerTrackSelectedInListBox(wxCommandEvent& event );

};

#endif
