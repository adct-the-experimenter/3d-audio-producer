#include "immediate_mode_sound_player.h"



ImmediateModeSoundPlayer::ImmediateModeSoundPlayer()
{
	m_state = IMSoundPlayerState::NONE;
	m_current_time = 0;
}

ImmediateModeSoundPlayer::~ImmediateModeSoundPlayer()
{
	
}

void ImmediateModeSoundPlayer::DrawGui_Item()
{
	
}

void ImmediateModeSoundPlayer::PlayAll()
{
	/*
	//buffer audio for all sound producer sources with buffers
	for(size_t i=0; i < soundProducerTracks_vec->size(); i++)
	{
		//ImmediateModeSoundPlayer::StartPlayerBuffering();
	}

	//play all sources in sync
	if(m_state == IMSoundPlayerState::NONE)
	{
		//start play the sources
		audioPlayer.PlayMultipleSources(&soundproducertracks_sources_vector);
	}
	else if(m_state == IMSoundPlayerState::PLAYING)
	{
		//play updated buffers of sources
		audioPlayer.PlayMultipleUpdatedPlayerBuffers(&soundproducertracks_sources_vector);
	}
	
	//buffering time is 500 milliseconds
	m_current_time += 500;
	*/
}

void  ImmediateModeSoundPlayer::al_nssleep(unsigned long nsec)
{
    struct timespec ts, rem;
    ts.tv_sec = nsec / 1000000000ul;
    ts.tv_nsec = nsec % 1000000000ul;
    while(nanosleep(&ts, &rem) == -1 && errno == EINTR){ts = rem;}
}

void ImmediateModeSoundPlayer::LoadBufferStreaming(ALuint* sourceToManipulatePtr)
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
