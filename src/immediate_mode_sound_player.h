#ifndef IM_SOUND_PLAYER
#define IM_SOUND_PLAYER

#include "sound_bank.h"
#include "openalsoft-player.h"
#include "soundproducer-registry.h"

#include "effects-manager.h"

class ImmediateModeSoundPlayer
{
public:
	
	ImmediateModeSoundPlayer();
	~ImmediateModeSoundPlayer();
	
	void RunStateForPlayer();
	
	//draw gui elements
	void DrawGui_Item();
	
	void SetPointerToSoundBank(SoundBank* soundbank_ptr);
	
	void SetPointerToSoundProducerRegistry(SoundProducerRegistry* sound_producer_reg);
	
	void SetPointerToEffectsManager(EffectsManager* effects_manager);
	
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
	
	void InitAudioPlayersForEachSoundProducer(size_t num_producers);
	
	//function to load buffer for
	//must call open streaming files before this.
	void LoadBufferStreaming(ALuint* sourceToManipulatePtr,OpenALSoftPlayer& audioPlayer);
	
	OpenALSoftPlayer mainAudioPlayer;
	
	std::vector <OpenALSoftPlayer> buffering_audio_players_vec;
	
	double m_current_time;
	
	void al_nssleep(unsigned long nsec);
	
	SoundBank* m_sound_bank_ptr;
	
	SoundProducerRegistry* m_sound_producer_reg_ptr;
	
	EffectsManager* m_effects_manager_ptr;
	
	//amonut of time to increment for playback
	double time_res_seconds;
};

#endif
