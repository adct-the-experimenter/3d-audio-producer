#ifndef IM_SOUND_PLAYER
#define IM_SOUND_PLAYER

#include "sound_bank.h"
#include "openalsoft-player.h"
#include "soundproducer-registry.h"

#include "effects-manager.h"

enum class BufferPlayerStateType : std::uint8_t {NONE=0,PLAYING, PAUSED, 
												NONE_TO_PLAYING, PAUSED_TO_PLAYING, 
												PLAYING_TO_NONE, PLAYING_TO_PAUSED,
												PAUSED_TO_NONE};
												
struct BufferPlayerState
{
	BufferPlayerStateType state_type;
	double current_time;
};

class ImmediateModeSoundPlayer
{
public:
	
	ImmediateModeSoundPlayer();
	~ImmediateModeSoundPlayer();
	
	
	
	//draw gui elements
	void DrawGui_Item();
	
	void SetPointerToSoundBank(SoundBank* soundbank_ptr);
	
	void SetPointerToSoundProducerRegistry(SoundProducerRegistry* sound_producer_reg);
	
	void SetPointerToEffectsManager(EffectsManager* effects_manager);
	
	enum class IMSoundPlayerState : std::uint8_t {NONE=0,PLAYING, PAUSED, REWINDING, FAST_FORWARDING };
	
	
	
	//function to indicate that player is not in none or paused state.
	bool PlayerInActiveUse();
	
	//function to set current time in sound player
	void SetCurrentTimeInSoundPlayer(double& time);
	
//playback functions

//simple playback all sounds played, stopped, paused at the same time
	//function to manage simple playback state
	void RunStateForPlayer_SimplePlayback();
	
	//play all sounds at the same time
	void PlayAll_SimplePlayback();
	
	//pause playing all sounds at the same time
	void PauseAll_SimplePlayback();
	
	//stop and restart all sounds at the same time
	void StopAll_SimplePlayback();
	
	

//complex playback, sounds are individually set to be played, paused, stopped
//all sounds to be played are played simultaneously for simultaneous sound
	
	//function to manage complex playback state
	void RunStateForPlayer_ComplexPlayback();
	 
	//play individual buffer player linked to sound producer
	void SetBufferPlayerToPlay_ComplexPlayback(int index);
	
	//pause individual buffer player linked to sound producer
	void SetBufferPlayerToPause_ComplexPlayback(int index);
	
	//stop and restart individual buffer player linked to sound producer
	void SetBufferPlayerToStop_ComplexPlayback(int index);
	
	//function to start playback in general, sounds determined 
	void StartPlayback_ComplexPlayback();
	
	//function to pause playback of all sounds
	void PausePlayback_ComplexPlayback();
	
	//function to stop playback of all sounds
	void StopPlayback_ComplexPlayback();
	
	void ResetPlayers_ComplexPlayback();
	
	//function to set time for buffer state in complex playback
	void SetCurrentTimeInBuffer_ComplexPlayback(int index, double& current_time);
	
	//function to initialize player buffer states and audio players
	void InitPlayer_ComplexPlayback();
	
private:
	
	//state of immmediate mode player
	IMSoundPlayerState m_state;
	
	void SetSoundPlayerState(IMSoundPlayerState state);
	IMSoundPlayerState GetSoundPlayerState();
	
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
	
	//amount of time to increment for playback
	double time_res_seconds;
	
	bool player_active_use;
	
	//complex playback specific variables and functions
	std::vector <BufferPlayerState> buffer_players_states;
	
	void InitStateForPlayers_ComplexPlayback(size_t num_producers);
	
	void Stop_IndividualBufferPlayer_ComplexPlayback(int index);
	void Pause_IndividualBufferPlayer_ComplexPlayback(int index);
	void Play_IndividualBufferPlayer_ComplexPlayback(int index);
	
	void TransitionNoneToPlay_IndividualBufferPlayer_ComplexPlayback(int index);
	void TransitionPausedToPlay_IndividualBufferPlayer_ComplexPlayback(int index);
	void TransitionPlayToNone_IndividualBufferPlayer_ComplexPlayback(int index);
	void TransitionPlayToPaused_IndividualBufferPlayer_ComplexPlayback(int index);
	
};

#endif
