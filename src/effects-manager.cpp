#include "effects-manager.h"

EffectsManager::EffectsManager()
{
	m_listener_ptr = nullptr;
	
	effect_zones_vector.clear();
	standard_reverb_zones_vector.clear();
	eax_reverb_zones_vector.clear();
	echo_zones_vector.clear();
	
}

EffectsManager::~EffectsManager()
{	
	m_listener_ptr = nullptr;
}

void EffectsManager::Draw3DModels()
{
	if(effect_zones_vector.size() > 0)
	{
		for(size_t i = 0; i < effect_zones_vector.size(); i++)
		{
			if(effect_zones_vector[i])
			{
				effect_zones_vector[i]->DrawModel();
			}
			
		}
	}
	
}

void EffectsManager::CreateStandardReverbZone(std::string& name, float& x, float& y, float& z, float& width, ReverbStandardProperties& properties)
{
	ReverbZone r_zone;
	
	standard_reverb_zones_vector.push_back(r_zone);
	
	standard_reverb_zones_vector.back().InitStandardReverbZoneWithGraphicalObject(name,x,y,z,width,properties);
	
	effect_zones_vector.push_back(&standard_reverb_zones_vector.back());
	
}


void EffectsManager::CreateEAXReverbZone(std::string& name, float& x, float& y, float& z, float& width, ReverbEAXProperties& properties)
{
	ReverbZone r_zone;
	
	eax_reverb_zones_vector.push_back(r_zone);
	
	eax_reverb_zones_vector.back().InitEAXReverbZoneWithGraphicalObject(name,x,y,z,width,properties);
	
	effect_zones_vector.push_back(&eax_reverb_zones_vector.back());
	
}

void EffectsManager::CreateEchoZone(std::string& name, float& x, float& y, float& z, float& width, EchoZoneProperties& properties)
{
	EchoZone e_zone;
	
	echo_zones_vector.push_back(e_zone);
	
	echo_zones_vector.back().InitEchoZoneWithGraphicalObject(name,x,y,z,width,properties);
	
	effect_zones_vector.push_back(&echo_zones_vector.back());
	
}

void EffectsManager::SetPointerToListener(Listener* listener){m_listener_ptr = listener;}

void EffectsManager::SetPointerToSoundProducerRegistry(SoundProducerRegistry* sound_producer_reg){m_sound_producer_reg_ptr = sound_producer_reg;}

std::vector <EffectZone*> *EffectsManager::GetReferenceToEffectZoneVector(){return &effect_zones_vector;}

EffectZone* EffectsManager::GetPointerToEffectZone(size_t& index){return effect_zones_vector[index];}

ReverbZone* EffectsManager::GetPointerToStandardReverbZone(size_t& index){return &standard_reverb_zones_vector[index];}
ReverbZone* EffectsManager::GetPointerToEAXReverbZone(size_t& index){return &eax_reverb_zones_vector[index];}

EchoZone* EffectsManager::GetPointerToEchoZone(size_t& index){return &echo_zones_vector[index];}

void EffectsManager::PerformReverbThreadOperation()
{
	
	//if there are effect zones
	if( effect_zones_vector.size() > 0 && m_sound_producer_reg_ptr != nullptr)
	{
		//for each effect zone
		for(size_t i = 0; i < effect_zones_vector.size(); i++)
		{
			//check if listener is in reverb zone
			EffectZone* thisZone = effect_zones_vector[i];
			
			//check if zone is initialized
			if( thisZone->GetWidth() != 0)
			{
				if(EffectsManager::IsListenerInThisEffectZone(thisZone))
				{
					//if listener is in the reverb zone
					
					//check if sound producers are inside the zone
					if(m_sound_producer_reg_ptr->sound_producer_sources_vec.size() > 0)
					{
						for(size_t i = 0; i < m_sound_producer_reg_ptr->sound_producer_vector_ref->size(); i++)
						{
							ALuint* source_ptr = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(i)->getSource();
							
							//if no reverb is applied
							if(!EffectsManager::DoesSourceHaveEffectApplied(source_ptr))
							{
								SoundProducer* thisSoundProducer = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(i).get();
							
								//if there is a sound producer attached to sound producer track
								if(thisSoundProducer != nullptr)
								{
									//if sound producer is inside the zone
									if(EffectsManager::IsThisSoundProducerInsideEffectZone(thisSoundProducer,thisZone))
									{
										//std::cout << "SoundProducer is inside the reverb zone!\n";
										//apply reverb to source of sound producer track
										EffectsManager::ApplyThisEffectZoneEffectToThisSource(source_ptr,thisZone);
									}
								}
							}
							
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					//if listener is not in the reverb zone
					
					for(size_t i = 0; i < m_sound_producer_reg_ptr->sound_producer_vector_ref->size(); i++)
					{						
						SoundProducer* thisSoundProducer = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(i).get();
						
						ALuint* source_ptr = m_sound_producer_reg_ptr->sound_producer_vector_ref->at(i)->getSource(); 
						
						//if there is a sound producer attached to sound producer track
						if(thisSoundProducer != nullptr)
						{
							//if sound producer is inside the zone
							if(EffectsManager::IsThisSoundProducerInsideEffectZone(thisSoundProducer,thisZone)
							   || EffectsManager::DoesSourceHaveEffectApplied(source_ptr))
							{
								//remove reverb effect from sound producer track
								EffectsManager::RemoveEffectFromThisSource(source_ptr);
							}
						}
					}
					
				}
				
			}
			
		}
		
	}
	
}

bool EffectsManager::IsListenerInThisEffectZone(EffectZone* thisZone)
{
	if(!m_listener_ptr){return false;}
	
	float zone_width = thisZone->GetWidth();
	float listener_width = 2.0f; //from sound producer class
	
	//all cubes, use same width in x,y,z direction
	
	if(CheckCollisionBoxes(
            (BoundingBox){(Vector3){ m_listener_ptr->getPositionX() - listener_width/2,
                                     m_listener_ptr->getPositionY() - listener_width/2,
                                     m_listener_ptr->getPositionZ() - listener_width/2 },
                          (Vector3){ m_listener_ptr->getPositionX() + listener_width/2,
                                     m_listener_ptr->getPositionY() + listener_width/2,
                                     m_listener_ptr->getPositionZ() + listener_width/2 }},
            (BoundingBox){(Vector3){ thisZone->GetPositionX() - zone_width/2,
                                     thisZone->GetPositionY() - zone_width/2,
                                     thisZone->GetPositionZ() - zone_width/2 },
                          (Vector3){ thisZone->GetPositionX() + zone_width/2,
                                     thisZone->GetPositionY() + zone_width/2,
                                     thisZone->GetPositionZ() + zone_width/2 }}  )
        )
	{
		return true;
	}
	
	return false;
	
	return false;
}

bool EffectsManager::IsThisSoundProducerInsideEffectZone(SoundProducer* thisSoundProducer,EffectZone* thisZone)
{
	float zone_width = thisZone->GetWidth();
	float producer_width = 2.0f; //from sound producer class
	
	//all cubes, use same width in x,y,z direction
	
	if(CheckCollisionBoxes(
            (BoundingBox){(Vector3){ thisSoundProducer->GetPositionX() - producer_width/2,
                                     thisSoundProducer->GetPositionY() - producer_width/2,
                                     thisSoundProducer->GetPositionZ() - producer_width/2 },
                          (Vector3){ thisSoundProducer->GetPositionX() + producer_width/2,
                                     thisSoundProducer->GetPositionY() + producer_width/2,
                                     thisSoundProducer->GetPositionZ() + producer_width/2 }},
            (BoundingBox){(Vector3){ thisZone->GetPositionX() - zone_width/2,
                                     thisZone->GetPositionY() - zone_width/2,
                                     thisZone->GetPositionZ() - zone_width/2 },
                          (Vector3){ thisZone->GetPositionX() + zone_width/2,
                                     thisZone->GetPositionY() + zone_width/2,
                                     thisZone->GetPositionZ() + zone_width/2 }}  )
        )
	{
		return true;
	}
	
	return false;
}

void EffectsManager::ApplyThisEffectZoneEffectToThisSource(ALuint* source, EffectZone* thisZone)
{
	// Connect the source to the effect slot. This tells the source to use the
	// effect slot 'slot', on send #0 with the AL_FILTER_NULL filter object.
	
	alSource3i(*source, AL_AUXILIARY_SEND_FILTER, (ALint)(*thisZone->GetEffectsSlotPointer()), 0, AL_FILTER_NULL);
	assert(alGetError()== AL_NO_ERROR && "Failed to setup effect for sound source send 0.");
}

void EffectsManager::RemoveEffectFromThisSource(ALuint* source)
{
	alSource3i(*source,AL_AUXILIARY_SEND_FILTER,AL_EFFECTSLOT_NULL, 0, 0); 
	assert(alGetError()==AL_NO_ERROR && "Failed to disable source send 0.");
}

/*
void EffectsManager::ApplyThisEffectZoneEffectToThisTrack(SoundProducerTrack* thisSoundProducerTrack, EffectZone* thisZone)
{
	
	// Connect the source to the effect slot. This tells the source to use the
	// effect slot 'slot', on send #0 with the AL_FILTER_NULL filter object.
	//
	
	ALuint* thisSource = thisSoundProducerTrack->GetReferenceToTrackSource();
	 
	alSource3i(*thisSource, AL_AUXILIARY_SEND_FILTER, (ALint)(*thisZone->GetEffectsSlotPointer()), 0, AL_FILTER_NULL);
	assert(alGetError()== AL_NO_ERROR && "Failed to setup effect for sound source send 0.");
	
	thisSoundProducerTrack->SetStatusReverbApplied(true);
}

void EffectsManager::RemoveEffectFromThisTrack(SoundProducerTrack* thisSoundProducerTrack)
{
	ALuint* thisSource = thisSoundProducerTrack->GetReferenceToTrackSource();
	
	alSource3i(*thisSource,AL_AUXILIARY_SEND_FILTER,AL_EFFECTSLOT_NULL, 0, 0); 
	assert(alGetError()==AL_NO_ERROR && "Failed to disable source send 0.");
	
	thisSoundProducerTrack->SetStatusReverbApplied(false);
}

std::vector <SoundProducerTrack*> *EffectsManager::GetReferenceToSoundProducerTracksVector()
{
	return m_track_manager_ptr->soundProducerTracks_vec;
}
*/

void EffectsManager::FreeEffects()
{
	std::cout << "Freeing effects...\n";
	for(size_t i=0; i < standard_reverb_zones_vector.size(); i++)
	{
		standard_reverb_zones_vector[i].FreeEffects();
	}
	
	for(size_t i=0; i < eax_reverb_zones_vector.size(); i++)
	{
		eax_reverb_zones_vector[i].FreeEffects();	
	}
	
	for(size_t i=0; i < echo_zones_vector.size(); i++)
	{
		echo_zones_vector[i].FreeEffects();
	}
}

bool EffectsManager::DoesSourceHaveEffectApplied(ALuint* source)
{
	//alGetSource3i(ALuint source, ALenum param, ALint *v1, ALint *v2, ALint *v3);
	ALint v1,v2,v3;
	alGetSource3i(*source, AL_AUXILIARY_SEND_FILTER, &v1, &v2, &v3);
	
	if(v1 == AL_EFFECTSLOT_NULL)
	{
		return false;
	}
	else
	{
		return true;
	} 
}
