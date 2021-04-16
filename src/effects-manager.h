#ifndef EFFECTS_MANAGER_H
#define EFFECTS_MANAGER_H

#include "soundproducer-registry.h"

#include "reverb-zone.h"
#include "echo-zone.h"

#include "listener.h"

//class to manipulate x,y z position of sound producer
class EffectsManager
{
public:
	EffectsManager();
	~EffectsManager();
	
	void Draw3DModels();
	
	void SetPointerToListener(Listener* listener);
	void SetPointerToSoundProducerRegistry(SoundProducerRegistry* sound_producer_reg);
	
	//function to create reverb zone that uses standard effects
	void CreateStandardReverbZone(std::string& name, float& x, float& y, float& z, float& width, ReverbStandardProperties& properties);
	
	//function to create reverb zone that uses EAX effectss
	void CreateEAXReverbZone(std::string& name, float& x, float& y, float& z, float& width, ReverbEAXProperties& properties);
	
	//function to create echo zone
	void CreateEchoZone(std::string& name, float& x, float& y, float& z, float& width, EchoZoneProperties& properties);
	
	//function to return a pointer to reverb zone vector
	std::vector <EffectZone*> *GetReferenceToEffectZoneVector();
	
	//function to return a pointer to reverb zone from index in vector
	EffectZone* GetPointerToEffectZone(size_t& index);
	ReverbZone* GetPointerToStandardReverbZone(size_t& index);
	ReverbZone* GetPointerToEAXReverbZone(size_t& index);
	EchoZone* GetPointerToEchoZone(size_t& index);
	
	//function to run to apply reverb zone effect if listener is in reverb zone
	void RunListenerInEffectZoneOperation();
	
	//function to free reverb zone effects
	void FreeEffects();
	
	
	friend class CheckListenerReverbZoneThread;
	
	friend class CreateEAXReverbZoneDialog;
	friend class CreateStandardReverbZoneDialog;
	friend class CreateEchoZoneDialog;

	friend class EditMultipleStandardReverbZonesDialog;
	friend class EditMultipleEAXReverbZonesDialog;
	friend class EditMultipleEchoZonesDialog;
	
	friend class SaveSystem;
	friend class LoadSystem;
	
private:
	
	//pointer to manager that contains all soundproducer tracks used
	//SoundProducerTrackManager* m_track_manager_ptr;
	
	//pointer to listener
	Listener* m_listener_ptr;
	
	//vector to contain many reverb zone objects
	std::vector <EffectZone*> effect_zones_vector;
	
	std::vector <ReverbZone> standard_reverb_zones_vector;
	std::vector <ReverbZone> eax_reverb_zones_vector;
	
	std::vector <EchoZone> echo_zones_vector;
	
	//function to perform the entire reverb thread operation of checking and setting reverb
	void PerformReverbThreadOperation();
	
	//function to return bool of if a listener is in a reverb zone
	bool IsListenerInThisEffectZone(EffectZone* thisZone);
	
	//function to return bool of if a sound producer is in a reverb zone
	bool IsThisSoundProducerInsideEffectZone(SoundProducer* thisSoundProducer,EffectZone* thisZone);
	
	//function to apply reverb effect of a zone to sound producer track
	//void ApplyThisEffectZoneEffectToThisTrack(SoundProducerTrack* thisSoundProducerTrack, EffectZone* thisZone);
	void ApplyThisEffectZoneEffectToThisSource(ALuint* source, EffectZone* thisZone);
	
	//function to remove effect applied to the sound producer track
	//void RemoveEffectFromThisTrack(SoundProducerTrack* thisSoundProducerTrack);
	void RemoveEffectFromThisSource(ALuint* source);
	
	//std::vector <SoundProducerTrack*> *GetReferenceToSoundProducerTracksVector();
	SoundProducerRegistry* m_sound_producer_reg_ptr;
	
	bool DoesSourceHaveEffectApplied(ALuint* source);
};



#endif
