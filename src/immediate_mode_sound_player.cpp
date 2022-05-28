#include "immediate_mode_sound_player.h"

#undef RAYGUI_IMPLEMENTATION

#include "raygui/raygui.h"

#include <thread> //for using thread
#include <chrono>

#define TIME_RESOLUTION 100

ImmediateModeSoundPlayer::ImmediateModeSoundPlayer()
{
	ImmediateModeSoundPlayer::SetSoundPlayerState(IMSoundPlayerState::NONE);
	m_current_time = 0;
	
	m_sound_bank_ptr = nullptr;
	m_sound_producer_reg_ptr = nullptr;
	
	time_res_seconds = double(TIME_RESOLUTION) / 1000;
	
	player_active_use = false;
	
	buffering_audio_players_vec.clear();
	buffer_players_states.clear();
}

ImmediateModeSoundPlayer::~ImmediateModeSoundPlayer()
{
	
}

void ImmediateModeSoundPlayer::SetPointerToSoundBank(SoundBank* soundbank_ptr){m_sound_bank_ptr = soundbank_ptr;}

void ImmediateModeSoundPlayer::SetPointerToSoundProducerRegistry(SoundProducerRegistry* sound_producer_reg){m_sound_producer_reg_ptr = sound_producer_reg;}

EffectsManager* local_effect_manager_ptr = nullptr;

void ImmediateModeSoundPlayer::SetPointerToEffectsManager(EffectsManager* effects_manager)
{
	m_effects_manager_ptr = effects_manager;
	local_effect_manager_ptr = effects_manager;
}

void ImmediateModeSoundPlayer::SetCurrentTimeInSoundPlayer(double& time){m_current_time = time;}

bool ImmediateModeSoundPlayer::PlayerInActiveUse(){return player_active_use;}

void ImmediateModeSoundPlayer::RunStateForPlayer_SimplePlayback()
{
	switch(m_state)
	{
		case IMSoundPlayerState::NONE:
		{
			//stop operation 
			if(m_state != IMSoundPlayerState::NONE)
			{
				m_state = IMSoundPlayerState::NONE;
				
				for(size_t it = 0; it < buffering_audio_players_vec.size(); it++)
				{
					std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(it)->GetAccountNumber();
					std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
					bool active = m_sound_bank_ptr->m_sound_accounts[account_num].active;
					
					if(!active){return;}
					
					if(stream_filepath != "")
					{
						buffering_audio_players_vec[it].OpenPlayerFile(stream_filepath.c_str());
						ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
						
						if(sourceToManipulatePtr != nullptr)
						{
							buffering_audio_players_vec[it].StopSource(sourceToManipulatePtr);
							buffering_audio_players_vec[it].RewindSource(sourceToManipulatePtr);
							buffering_audio_players_vec[it].ClearQueue(sourceToManipulatePtr);
						}
					}
				}
				
			} 
			break;
		}
		case IMSoundPlayerState::PLAYING:{ ImmediateModeSoundPlayer::PlayAll_SimplePlayback(); break;}
		case IMSoundPlayerState::PAUSED:{ ImmediateModeSoundPlayer::PauseAll_SimplePlayback(); break;}
		case IMSoundPlayerState::FAST_FORWARDING:{ break;}
		case IMSoundPlayerState::REWINDING:{ break;}
	}
}

void ImmediateModeSoundPlayer::DrawGui_Item()
{

	
}



void DetermineEffect()
{
    if(local_effect_manager_ptr)
    {
		local_effect_manager_ptr->PerformEffectThreadOperation();
	}
}


void ImmediateModeSoundPlayer::PlayAll_SimplePlayback()
{
	player_active_use = true;
	
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;
	
	
	//launch worker thread to apply certain effect if listener is in a certain effect zone
    std::thread effect_worker_thread(DetermineEffect);
      
	if(m_state == IMSoundPlayerState::NONE)
	{
		
		if(m_sound_producer_reg_ptr && m_sound_bank_ptr)
		{
			//initialize audio players for each sound producer
			size_t num_producers = m_sound_producer_reg_ptr->sound_producer_vector_ref->size();
			ImmediateModeSoundPlayer::InitAudioPlayersForEachSoundProducer(num_producers);
			
			//open streaming file linked to sound bank account of sound producers
			for(size_t it = 0; it < buffering_audio_players_vec.size(); it++)
			{
				std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(it)->GetAccountNumber();
				std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
				
				bool active = m_sound_bank_ptr->m_sound_accounts[account_num].active;
				if(!active){continue;}
				
				if(stream_filepath != "")
				{
					buffering_audio_players_vec[it].OpenPlayerFile(stream_filepath.c_str());
					ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
					ImmediateModeSoundPlayer::LoadBufferStreaming(sourceToManipulatePtr,buffering_audio_players_vec[it]);
				}
				
			}
			
		}
		
		//wait for worker thread to finish effect operation
		effect_worker_thread.join();
		
		//start play the sources
		mainAudioPlayer.PlayMultipleSources(&m_sound_producer_reg_ptr->sound_producer_sources_vec);
  	
	}
	else if(m_state == IMSoundPlayerState::PAUSED)
	{
		//wait for worker thread to finish effect operation
		effect_worker_thread.join();
		
		for(size_t it = 0; it < buffering_audio_players_vec.size(); it++)
		{
			std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(it)->GetAccountNumber();
			
			bool active = m_sound_bank_ptr->m_sound_accounts[account_num].active;
			if(!active){continue;}
			
			std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
			
			if(stream_filepath != "")
			{
				ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
				buffering_audio_players_vec[it].ClearQueue(sourceToManipulatePtr);
				buffering_audio_players_vec[it].StartPlayerBuffering(sourceToManipulatePtr,m_current_time);
			}	
			
		}
	}
	else
	{
		clock::time_point start_time = clock::now();
		
		//buffer audio for all sound producer sources with buffer audio players
		for(size_t it = 0; it < buffering_audio_players_vec.size(); it++)
		{
			ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
			ImmediateModeSoundPlayer::LoadBufferStreaming(sourceToManipulatePtr,buffering_audio_players_vec[it]);
		}

		//wait for worker thread to finish effect operation
		effect_worker_thread.join();
		
		//play all sources in sync
		if(m_state == IMSoundPlayerState::PLAYING)
		{
			//play updated buffers of sources
			mainAudioPlayer.PlayMultipleUpdatedPlayerBuffers(&m_sound_producer_reg_ptr->sound_producer_sources_vec);
		}
		
		clock::time_point end_time = clock::now();
		
		//buffering time is 500 milliseconds
		duration time_inc = end_time - start_time;
		time_res_seconds = static_cast <double> (time_inc.count() / 1000); 
		m_current_time += time_res_seconds;
		
		
		//std::cout << "\n current time: " << m_current_time << std::endl;
		
	}
	
	m_state = IMSoundPlayerState::PLAYING;
	
}

//pause playing sounds
void ImmediateModeSoundPlayer::PauseAll_SimplePlayback()
{
	
	if(m_state != IMSoundPlayerState::PAUSED)
	{
		m_state = IMSoundPlayerState::PAUSED;
		
		for(size_t it = 0; it < buffering_audio_players_vec.size(); it++)
		{
			std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(it)->GetAccountNumber();
			
			bool active = m_sound_bank_ptr->m_sound_accounts[account_num].active;
			if(!active){continue;}
			
			std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
			
			if(stream_filepath != "")
			{
				ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
				buffering_audio_players_vec[it].RewindSource(sourceToManipulatePtr);
				buffering_audio_players_vec[it].ClearQueue(sourceToManipulatePtr);
			}
			
		}
		
		m_current_time += time_res_seconds;
		
		player_active_use = false;
		m_effects_manager_ptr->RemoveEffectFromAllSources();
	}
	
}
	
//stop and restart all sounds
void ImmediateModeSoundPlayer::StopAll_SimplePlayback()
{
	for(size_t it = 0; it < buffering_audio_players_vec.size(); it++)
	{
		std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(it)->GetAccountNumber();
		
		bool active = m_sound_bank_ptr->m_sound_accounts[account_num].active;
		if(!active){continue;}
		
		std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
		
		if(stream_filepath != "")
		{
			ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
			if(sourceToManipulatePtr)
			{
				buffering_audio_players_vec[it].StopSource(sourceToManipulatePtr);
				buffering_audio_players_vec[it].RewindSource(sourceToManipulatePtr);
				buffering_audio_players_vec[it].ClearQueue(sourceToManipulatePtr);
			}
		}
		
	}
	
	player_active_use = false;
	m_effects_manager_ptr->RemoveEffectFromAllSources();
	
	m_current_time = 0;
	m_state = IMSoundPlayerState::NONE;
}


static bool player_started = false;


void ImmediateModeSoundPlayer::RunStateForPlayer_ComplexPlayback()
{
	//if player is not active i.e. playback is not happening, then do nothing
	if(ImmediateModeSoundPlayer::GetSoundPlayerState() != IMSoundPlayerState::PLAYING){return;}
	
	//initialize vector for sources to play
	std::vector <ALuint*> sources_to_continue_playing_vec;
	
	
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;
	
	clock::time_point start_time = clock::now();
	
	//launch worker thread to apply certain effect if listener is in a certain effect zone
    std::thread effect_worker_thread(DetermineEffect);
    
	//for all buffer player, depending on state
	for(int it = 0; it < buffer_players_states.size(); it++)
	{
		switch(buffer_players_states[it].state_type)
		{
			case BufferPlayerStateType::PLAYING_TO_NONE:
			{
				ImmediateModeSoundPlayer::TransitionPlayToNone_IndividualBufferPlayer_ComplexPlayback(it);
				buffer_players_states[it].state_type = BufferPlayerStateType::NONE;
				buffer_players_states[it].current_time = 0;
				break;
			}
			case BufferPlayerStateType::NONE:
			{
				//start player buffering
				break;
			}
			case BufferPlayerStateType::NONE_TO_PLAYING:
			{
				ImmediateModeSoundPlayer::TransitionNoneToPlay_IndividualBufferPlayer_ComplexPlayback(it);
				
				ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
				sources_to_continue_playing_vec.push_back(sourceToManipulatePtr);
				
				buffer_players_states[it].state_type = BufferPlayerStateType::PLAYING;
				
				break;
			}
			case BufferPlayerStateType::PAUSED_TO_PLAYING:
			{
				ImmediateModeSoundPlayer::TransitionPausedToPlay_IndividualBufferPlayer_ComplexPlayback(it);
				
				ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
				sources_to_continue_playing_vec.push_back(sourceToManipulatePtr);
				
				buffer_players_states[it].state_type = BufferPlayerStateType::PLAYING;
				break;
			}
			case BufferPlayerStateType::PLAYING:
			{
				ImmediateModeSoundPlayer::Play_IndividualBufferPlayer_ComplexPlayback(it);
				
				ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
				sources_to_continue_playing_vec.push_back(sourceToManipulatePtr);
				break;
			}
			case BufferPlayerStateType::PLAYING_TO_PAUSED:
			{
				ImmediateModeSoundPlayer::TransitionPlayToPaused_IndividualBufferPlayer_ComplexPlayback(it);
				
				buffer_players_states[it].state_type = BufferPlayerStateType::PAUSED;
				
				break;
			}
			case BufferPlayerStateType::PAUSED:
			{
				
				break;
			}
			default:{break;}
			
		}
	}
	
	//wait for worker thread to finish effect operation
	effect_worker_thread.join();
	
	//play all sources in sync
	
	//play updated buffers of sources
	if(sources_to_continue_playing_vec.size() > 0)
	{
		mainAudioPlayer.PlayMultipleUpdatedPlayerBuffers(&sources_to_continue_playing_vec);
	}
	
	
	//increment time for playback 
	
	clock::time_point end_time = clock::now();
	
	//buffering time is 500 milliseconds
	duration time_inc = end_time - start_time;
	time_res_seconds = static_cast <double> (time_inc.count() / 1000); 
	
	for(size_t b_it = 0; b_it < buffer_players_states.size(); b_it++)
	{
		if( buffer_players_states[b_it].state_type == BufferPlayerStateType::PLAYING)
		{
			buffer_players_states[b_it].current_time += time_res_seconds;
			//std::cout << "buffer player index: " << b_it << " , time: " << buffer_players_states[b_it].current_time 
			//<< std::endl;
		}
	}
	
}


void ImmediateModeSoundPlayer::StartPlayback_ComplexPlayback()
{
	//if starting from stopped / null state or paused state
	if(ImmediateModeSoundPlayer::GetSoundPlayerState() == IMSoundPlayerState::NONE || 
		ImmediateModeSoundPlayer::GetSoundPlayerState() == IMSoundPlayerState::PAUSED)
	{
		//initialize audio players for each sound producer
		ImmediateModeSoundPlayer::InitPlayer_ComplexPlayback();
	
		//open streaming file linked to sound bank account of sound producers
		for(size_t it = 0; it < buffering_audio_players_vec.size(); it++)
		{
			std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(it)->GetAccountNumber();
			std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
			
			if(stream_filepath != "")
			{
				buffering_audio_players_vec[it].OpenPlayerFile(stream_filepath.c_str());
				ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
				
				double& current_time = buffer_players_states[it].current_time;
				buffering_audio_players_vec[it].StartPlayerBuffering(sourceToManipulatePtr,current_time); //start player
			}
			
		}
		
		player_started = true;
	}
	
	
	player_active_use = true;
	ImmediateModeSoundPlayer::SetSoundPlayerState(IMSoundPlayerState::PLAYING);
}
	
void ImmediateModeSoundPlayer::PausePlayback_ComplexPlayback()
{
	if(ImmediateModeSoundPlayer::GetSoundPlayerState() == IMSoundPlayerState::PLAYING)
	{
		player_active_use = false;
	
		for(size_t it = 0; it < buffering_audio_players_vec.size(); it++)
		{
			std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(it)->GetAccountNumber();
			std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
			
			if(stream_filepath != "")
			{
				ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
				buffering_audio_players_vec[it].RewindSource(sourceToManipulatePtr);
				buffering_audio_players_vec[it].ClearQueue(sourceToManipulatePtr);
			}
			
		}
		
		m_effects_manager_ptr->RemoveEffectFromAllSources();
		
		for(size_t b_it = 0; b_it < buffer_players_states.size(); b_it++)
		{
			BufferPlayerStateType new_state_type = buffer_players_states[b_it].state_type;
				
			//set up for later playback when in play mode
			switch(buffer_players_states[b_it].state_type)
			{
				//play later
				case BufferPlayerStateType::PLAYING:{ new_state_type = BufferPlayerStateType::PAUSED_TO_PLAYING; break;}
				//keep paused for later
				case BufferPlayerStateType::PAUSED:{ new_state_type = BufferPlayerStateType::PAUSED; break;}
				//do nothing
				default:{break;}
			}
			
			buffer_players_states[b_it].state_type = new_state_type;
		}
	}
	
	ImmediateModeSoundPlayer::SetSoundPlayerState(IMSoundPlayerState::PAUSED);
	
}
	
void ImmediateModeSoundPlayer::StopPlayback_ComplexPlayback()
{
	if(ImmediateModeSoundPlayer::GetSoundPlayerState() == IMSoundPlayerState::PLAYING || 
		ImmediateModeSoundPlayer::GetSoundPlayerState() == IMSoundPlayerState::PAUSED)
	{
		player_active_use = false;
		player_started = false;
		
		for(size_t it = 0; it < buffering_audio_players_vec.size(); it++)
		{
			std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(it)->GetAccountNumber();
			std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
			
			if(stream_filepath != "")
			{
				ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[it];
				if(sourceToManipulatePtr)
				{
					buffering_audio_players_vec[it].StopSource(sourceToManipulatePtr);
					buffering_audio_players_vec[it].RewindSource(sourceToManipulatePtr);
					buffering_audio_players_vec[it].ClearQueue(sourceToManipulatePtr);
				}
			}
			
		}
		
		m_effects_manager_ptr->RemoveEffectFromAllSources();
		
		for(size_t b_it = 0; b_it < buffer_players_states.size(); b_it++)
		{
			buffer_players_states[b_it].state_type = BufferPlayerStateType::NONE;
			buffer_players_states[b_it].current_time = 0;
		}
	}
	
	ImmediateModeSoundPlayer::SetSoundPlayerState(IMSoundPlayerState::NONE);
}

void ImmediateModeSoundPlayer::Stop_IndividualBufferPlayer_ComplexPlayback(int index)
{
	
}

void ImmediateModeSoundPlayer::Pause_IndividualBufferPlayer_ComplexPlayback(int index)
{
	
}

void ImmediateModeSoundPlayer::Play_IndividualBufferPlayer_ComplexPlayback(int index)
{		
	//buffer audio for all sound producer sources with buffer audio players
	ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[index];
	
	double& current_time = buffer_players_states[index].current_time;
	
	switch(buffering_audio_players_vec[index].UpdatePlayerBuffer(sourceToManipulatePtr,current_time))
	{
		case OpenALSoftPlayer::PlayerStatus::PLAYBACK_FINISHED:
		{
			std::cout << "Playback finished! \n";
			
			//stop
			buffer_players_states[index].state_type = BufferPlayerStateType::PLAYING_TO_NONE;
			break;
		}
		case OpenALSoftPlayer::PlayerStatus::FAILED_TO_READ_ANYMORE_AUDIO_FROM_FILE:
		{
			std::cout << "No more audio to read! \n";
			
			//stop
			buffer_players_states[index].state_type = BufferPlayerStateType::PLAYING_TO_NONE;
			
			break;
		}
		case OpenALSoftPlayer::PlayerStatus::GOOD_UPDATE_BUFFER_STATUS:
		{						
			break;
		}
	}
	
	al_nssleep(10000000);

}

void ImmediateModeSoundPlayer::TransitionNoneToPlay_IndividualBufferPlayer_ComplexPlayback(int index)
{
	if(m_sound_producer_reg_ptr && m_sound_bank_ptr)
	{
		
		//open streaming file linked to sound bank account of sound producers
		std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(index)->GetAccountNumber();
		std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
		
		if(stream_filepath != "")
		{
			
			ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[index];
			
			//start player since transitioning from none to playing
			double& current_time = buffer_players_states[index].current_time;
			
			buffering_audio_players_vec[index].StartPlayerBuffering(sourceToManipulatePtr,current_time); 
			al_nssleep(10000000);
		}
		
	}
	
}

void ImmediateModeSoundPlayer::TransitionPausedToPlay_IndividualBufferPlayer_ComplexPlayback(int index)
{
	std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(index)->GetAccountNumber();
	std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
	
	if(stream_filepath != "")
	{
		ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[index];
		buffering_audio_players_vec[index].ClearQueue(sourceToManipulatePtr);
		
		double& current_time = buffer_players_states[index].current_time;
		
		buffering_audio_players_vec[index].ClearQueue(sourceToManipulatePtr);
		buffering_audio_players_vec[index].StartPlayerBuffering(sourceToManipulatePtr,current_time);
		
	}
	
	al_nssleep(10000000);
}

void ImmediateModeSoundPlayer::TransitionPlayToNone_IndividualBufferPlayer_ComplexPlayback(int index)
{
	std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(index)->GetAccountNumber();
	std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
	
	if(stream_filepath != "")
	{
		ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[index];
		buffering_audio_players_vec[index].StopSource(sourceToManipulatePtr);
		buffering_audio_players_vec[index].RewindSource(sourceToManipulatePtr);
		buffering_audio_players_vec[index].ClearQueue(sourceToManipulatePtr);
	}
	
	m_effects_manager_ptr->RemoveEffectFromSoundProducer(index);
	al_nssleep(10000000);
}

void ImmediateModeSoundPlayer::TransitionPlayToPaused_IndividualBufferPlayer_ComplexPlayback(int index)
{
	std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(index)->GetAccountNumber();
	std::string stream_filepath = m_sound_bank_ptr->m_sound_accounts[account_num].stream_file_path;
	
	if(stream_filepath != "")
	{
		ALuint* sourceToManipulatePtr =  m_sound_producer_reg_ptr->sound_producer_sources_vec[index];
		buffering_audio_players_vec[index].RewindSource(sourceToManipulatePtr);
		buffering_audio_players_vec[index].ClearQueue(sourceToManipulatePtr);
	}
	
	m_effects_manager_ptr->RemoveEffectFromSoundProducer(index);
	al_nssleep(10000000);
}

void ImmediateModeSoundPlayer::SetBufferPlayerToPlay_ComplexPlayback(int index)
{
	auto* buffer_player_state_ptr = &buffer_players_states[index].state_type;
	
	switch(*buffer_player_state_ptr)
	{
		case BufferPlayerStateType::NONE:{ *buffer_player_state_ptr = BufferPlayerStateType::NONE_TO_PLAYING; break;}
		case BufferPlayerStateType::PAUSED:{ *buffer_player_state_ptr = BufferPlayerStateType::PAUSED_TO_PLAYING; break;}
		default:{break;}
	}
}
	
void ImmediateModeSoundPlayer::SetBufferPlayerToPause_ComplexPlayback(int index)
{
	auto* buffer_player_state_ptr = &buffer_players_states[index].state_type;
	
	switch(*buffer_player_state_ptr)
	{
		case BufferPlayerStateType::PLAYING:{ *buffer_player_state_ptr = BufferPlayerStateType::PLAYING_TO_PAUSED; break;}
		default:{break;}
	}
}
	
void ImmediateModeSoundPlayer::SetBufferPlayerToStop_ComplexPlayback(int index)
{
	auto* buffer_player_state_ptr = &buffer_players_states[index].state_type;
	
	switch(*buffer_player_state_ptr)
	{
		case BufferPlayerStateType::PAUSED:{ *buffer_player_state_ptr = BufferPlayerStateType::PAUSED_TO_NONE; break;}
		case BufferPlayerStateType::PLAYING:{ *buffer_player_state_ptr = BufferPlayerStateType::PLAYING_TO_NONE; break;}
		default:{break;}
	}
}

void ImmediateModeSoundPlayer::ResetPlayers_ComplexPlayback()
{
	ImmediateModeSoundPlayer::SetSoundPlayerState(IMSoundPlayerState::NONE);
	m_current_time = 0;
		
	time_res_seconds = double(TIME_RESOLUTION) / 1000;
	
	player_active_use = false;
	player_started = false;
	
	buffering_audio_players_vec.clear();
	buffer_players_states.clear();
}

void ImmediateModeSoundPlayer::al_nssleep(unsigned long nsec)
{
    struct timespec ts, rem;
    ts.tv_sec = nsec / 1000000000ul;
    ts.tv_nsec = nsec % 1000000000ul;
    while(nanosleep(&ts, &rem) == -1 && errno == EINTR){ts = rem;}
}

void ImmediateModeSoundPlayer::LoadBufferStreaming(ALuint* sourceToManipulatePtr,OpenALSoftPlayer& audioPlayer)
{
	if(sourceToManipulatePtr != nullptr)
	{
		
		//get current state and do things differently depending on current state
		switch(m_state)
		{
			//if just started playing
			case IMSoundPlayerState::NONE:
			{
				audioPlayer.StartPlayerBuffering(sourceToManipulatePtr,m_current_time); //start player
				
				break;
			}
			//if playing after pause
			case IMSoundPlayerState::PAUSED:
			{
				audioPlayer.ClearQueue(sourceToManipulatePtr);
				audioPlayer.StartPlayerBuffering(sourceToManipulatePtr,m_current_time);
				
				al_nssleep(10000000);
				break;
			}
			//if already in state of playing
			case IMSoundPlayerState::PLAYING:
			{
				switch(audioPlayer.UpdatePlayerBuffer(sourceToManipulatePtr,m_current_time))
				{
					case OpenALSoftPlayer::PlayerStatus::PLAYBACK_FINISHED:
					{
						std::cout << "Playback finished! \n";
						
						//stop
						m_state = IMSoundPlayerState::NONE;
						return;
						break;
					}
					case OpenALSoftPlayer::PlayerStatus::FAILED_TO_READ_ANYMORE_AUDIO_FROM_FILE:
					{
						std::cout << "No more audio to read! \n";
						
						//stop
						m_state = IMSoundPlayerState::NONE;
						return;
						break;
					}
					case OpenALSoftPlayer::PlayerStatus::GOOD_UPDATE_BUFFER_STATUS:
					{						
						break;
					}
				}
				
				al_nssleep(10000000);
				break;
			}
			//if rewinding
			case IMSoundPlayerState::REWINDING:
			{
				audioPlayer.StartPlayerBuffering(sourceToManipulatePtr,m_current_time);
				
				al_nssleep(10000000);
				break;
			}
			//if fast forwarding
			case IMSoundPlayerState::FAST_FORWARDING:
			{
				audioPlayer.StartPlayerBuffering(sourceToManipulatePtr,m_current_time);
				
				al_nssleep(10000000);
				break;
			}
			
		}		
	}
	
	//switch to player playing state
	m_state = IMSoundPlayerState::PLAYING; 
}

void ImmediateModeSoundPlayer::InitAudioPlayersForEachSoundProducer(size_t num_producers)
{
	if(num_producers != buffering_audio_players_vec.size())
	{
		buffering_audio_players_vec.clear();
		buffering_audio_players_vec.resize(num_producers);
		
		for(size_t i = 0; i < buffering_audio_players_vec.size(); i++)
		{
			buffering_audio_players_vec[i].InitBuffersForStreaming();
		}
	}
}

void ImmediateModeSoundPlayer::InitStateForPlayers_ComplexPlayback(size_t num_producers)
{
	if(num_producers != buffer_players_states.size())
	{
		buffer_players_states.clear();
		buffer_players_states.resize(num_producers);
	
		//set all to none state a.k.a. stopped
		for(size_t it = 0; it < buffer_players_states.size(); it++)
		{
			buffer_players_states[it].state_type = BufferPlayerStateType::NONE;
			buffer_players_states[it].current_time = 0;
		}
	}
	
}

void ImmediateModeSoundPlayer::SetSoundPlayerState(IMSoundPlayerState state){m_state = state;}
ImmediateModeSoundPlayer::IMSoundPlayerState ImmediateModeSoundPlayer::GetSoundPlayerState(){return m_state;}

void ImmediateModeSoundPlayer::SetCurrentTimeInBuffer_ComplexPlayback(int index, double& current_time)
{
	buffer_players_states[index].current_time = current_time;
}

void ImmediateModeSoundPlayer::InitPlayer_ComplexPlayback()
{
	size_t num_producers = m_sound_producer_reg_ptr->sound_producer_vector_ref->size();
	
	ImmediateModeSoundPlayer::InitAudioPlayersForEachSoundProducer(num_producers);
	ImmediateModeSoundPlayer::InitStateForPlayers_ComplexPlayback(num_producers);
}
