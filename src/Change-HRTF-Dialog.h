#ifndef _CHANGE_HRTF_DIALOG_H
#define _CHANGE_HRTF_DIALOG_H

#include "openalsoftaudioengine.h" //for hrtf operations



class ChangeHRTFDialog
{

public:
	ChangeHRTFDialog();

	void SetPointerToAudioEngine(OpenAlSoftAudioEngine* audioEngine);
	
	void InitGUI();
	
	void DrawDialog();

	void resetConfig();
	
	bool OkClickedOn();
	
private:
	std::vector <std::string> hrtf_names;

	int hrtf_selection_index;
	
	OpenAlSoftAudioEngine* ptrAudioEngine;
	
	bool okClicked;

};



#endif
