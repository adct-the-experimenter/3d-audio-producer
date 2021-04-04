#ifndef _HRTF_TEST_DIALOG_H
#define _HRTF_TEST_DIALOG_H

#include "openalsoftaudioengine.h" //for loading buffer and creating source of sound producer

class HRTFTestDialog
{

public:
	HRTFTestDialog();

	void SetPointerToAudioEngine(OpenAlSoftAudioEngine* audioEngine);

	void InitGUI();
	
	bool OkClickedOn();
	
	void DrawDialog();
	
	void resetConfig();
	
private:

	OpenAlSoftAudioEngine* ptrAudioEngine;
	bool okClicked;
	
};



#endif
