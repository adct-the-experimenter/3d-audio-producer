#include "immediate_mode_sound_player.h"

#undef RAYGUI_IMPLEMENTATION

#include "raygui/raygui.h"

#include <thread> //for using thread
#include <chrono>

#define TIME_RESOLUTION 100

ImmediateModeSoundPlayer::ImmediateModeSoundPlayer()
{
	m_state = IMSoundPlayerState::NONE;
	m_current_time = 0;
	
	m_sound_bank_ptr = nullptr;
	m_sound_producer_reg_ptr = nullptr;
	
	time_res_seconds = double(TIME_RESOLUTION) / 1000;
	
	player_active_use = false;
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

bool ImmediateModeSoundPlayer::PlayerInActiveUse(){return player_active_use;}

void ImmediateModeSoundPlayer::RunStateForPlayer()
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
		case IMSoundPlayerState::PLAYING:{ ImmediateModeSoundPlayer::PlayAll(); break;}
		case IMSoundPlayerState::PAUSED:{ ImmediateModeSoundPlayer::PauseAll(); break;}
		case IMSoundPlayerState::FAST_FORWARDING:{ break;}
		case IMSoundPlayerState::REWINDING:{ break;}
	}
}

void ImmediateModeSoundPlayer::DrawGui_Item()
{
	float center_x = GetScreenWidth()*0.5;
	
	//draw play button
	if(GuiButton( (Rectangle){ center_x - 100, 50, 50, 30 }, "Play" ))
	{
		ImmediateModeSoundPlayer::PlayAll();
		m_state = IMSoundPlayerState::PLAYING;
	}
	
	//draw pause button
	if(GuiButton( (Rectangle){ center_x, 50, 50, 30 },  "Pause" ))
	{
		if(m_state != IMSoundPlayerState::PAUSED){ImmediateModeSoundPlayer::PauseAll();}
		m_state = IMSoundPlayerState::PAUSED;
	}
	
	//draw stop button
	if(GuiButton( (Rectangle){ center_x + 100, 50, 50, 30 }, "Stop"))
	{
		ImmediateModeSoundPlayer::StopAll();
		m_state = IMSoundPlayerState::NONE;
	}
	
}



void DetermineEffect()
{
    if(local_effect_manager_ptr)
    {
		local_effect_manager_ptr->PerformEffectThreadOperation();
	}
}


void ImmediateModeSoundPlayer::PlayAll()
{
	player_active_use = true;
	
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<float, std::milli> duration;
	
	
	//launch worker thread to apply certain effect if listener is in a certain effect zone
    std::thread effect_worker_thread(DetermineEffect);
  
	//m_effects_manager_ptr->PerformEffectThreadOperation();
    
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
  	
	}
	else if(m_state == IMSoundPlayerState::PAUSED)
	{
		//wait for worker thread to finish effect operation
		effect_worker_thread.join();
		
		for(size_t it = 0; it < buffering_audio_players_vec.size(); it++)
		{
			std::uint8_t account_num = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(it)->GetAccountNumber();
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
		if(m_state == IMSoundPlayerState::NONE)
		{
			//start play the sources
			mainAudioPlayer.PlayMultipleSources(&m_sound_producer_reg_ptr->sound_producer_sources_vec);
		}
		else if(m_state == IMSoundPlayerState::PLAYING)
		{
			//play updated buffers of sources
			mainAudioPlayer.PlayMultipleUpdatedPlayerBuffers(&m_sound_producer_reg_ptr->sound_producer_sources_vec);
		}
		
		clock::time_point end_time = clock::now();
		
		//buffering time is 500 milliseconds
		//m_current_time += time_res_seconds;
		duration time_inc = end_time - start_time;
		time_res_seconds = static_cast <double> (time_inc.count() / 1000); 
		m_current_time += time_res_seconds;
		
		
		//std::cout << "\n current time: " << m_current_time << std::endl;
		
	}
	
	
	
}

//pause playing sounds
void ImmediateModeSoundPlayer::PauseAll()
{
	
	if(m_state != IMSoundPlayerState::PAUSED)
	{
		m_state = IMSoundPlayerState::PAUSED;
		
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
		
		m_current_time += time_res_seconds;
		
		player_active_use = false;
		m_effects_manager_ptr->RemoveEffectFromAllSources();
	}
	
}
	
//stop and restart all sounds
void ImmediateModeSoundPlayer::StopAll()
{
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
	
	player_active_use = false;
	m_effects_manager_ptr->RemoveEffectFromAllSources();
	
	m_current_time = 0;
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
