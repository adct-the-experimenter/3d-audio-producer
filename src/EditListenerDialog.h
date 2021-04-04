#ifndef _EDIT_LISTENER_H
#define _EDIT_LISTENER_H

#include <memory>

#include "listener.h"

class EditListenerDialog
{

public:
	EditListenerDialog(const std::string& title);
	
	void SetPointerToListener(Listener* listener);
	void InitGUI();
	void DrawDialog();
	
	void resetConfig();
	
	bool OkClickedOn();
	bool CancelClickedOn();
	
private:

	Listener* ptrListener;
	void ChangeListenerAttributes();
	
	bool okClicked; //bool to indicate if ok button was clicked on
	bool cancelClicked;
	
};



#endif
