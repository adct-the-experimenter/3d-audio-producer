#ifndef IM_SOUND_PLAYER
#define IM_SOUND_PLAYER

#include "sound_bank.h"
#include "openalsoft-player.h"

class ImmediateModeSoundPlayer
{
public:
	
	ImmediateModeSoundPlayer();
	~ImmediateModeSoundPlayer();
	
	//draw gui elements
	void DrawGui_Item();
	
	enum class IMSoundPlayerState : std::uint8_t {NONE=0,PLAYING, PAUSED, REWINDING, FAST_FORWARDING };
	
private:
	
	//state of immmediate mode player
	IMSoundPlayerState m_state;
	
	//play all sounds
	void PlayAll();
	
	//pause playing sounds
	void PauseAll();
	
	//stop and restart all sounds
	void StopAll();
	
	//thread for effect
	
	void LoadBufferStreaming(ALuint* sourceToManipulatePtr);
	
	OpenALSoftPlayer audioPlayer;
	double m_current_time;
	
	void al_nssleep(unsigned long nsec);
};

#endif
