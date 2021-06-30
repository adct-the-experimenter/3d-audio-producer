#ifndef EFFECTS_MANAGER_H
#define EFFECTS_MANAGER_H

#include "soundproducer-registry.h"

#include "reverb-zone.h"
#include "echo-zone.h"

#include "listener.h"

#include "raylib.h" //for Ray and Bounding Box

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
	
	
	//function to return a pointer to reverb zone from index in vector
	ReverbZone* GetPointerToStandardReverbZone(size_t& index);
	ReverbZone* GetPointerToEAXReverbZone(size_t& index);
	EchoZone* GetPointerToEchoZone(size_t& index);
	
	//function to run to apply reverb zone effect if listener is in reverb zone
	void RunListenerInEffectZoneOperation();
	
	//function to free reverb zone effects
	void FreeEffects();
	
	//function to perform the entire reverb thread operation of checking and setting reverb
	void PerformEffectThreadOperation();
	
	enum class EffectZoneType : std::uint8_t {NONE=0,STANDARD_REVERB,EAX_REVERB,ECHO};
	
	//function to return index and type of effects zone picked by 3d picking
	void CheckEffectZones3DPicking(Ray& picker_ray, EffectZoneType& type, int& index);
	
	void SetEffectZonePicked(bool state,EffectZoneType& type, int& index);
	
	void RemoveStandardReverbZone(int& index);
	void RemoveEAXReverbZone(int& index);
	void RemoveEchoZone(int& index);
	
	friend class CreateEAXReverbZoneDialog;
	friend class CreateStandardReverbZoneDialog;
	friend class CreateEchoZoneDialog;

	friend class EditMultipleStandardReverbZonesDialog;
	friend class EditMultipleEAXReverbZonesDialog;
	friend class EditMultipleEchoZonesDialog;
	
	friend class SaveSystem;
	friend class LoadSystem;
	
	void RemoveEffectFromAllSources();
	
private:
	
	//pointer to manager that contains all soundproducer tracks used
	//SoundProducerTrackManager* m_track_manager_ptr;
	
	//pointer to listener
	Listener* m_listener_ptr;
	
	//effect zone vectors
	std::vector <ReverbZone> standard_reverb_zones_vector;
	std::vector <ReverbZone> eax_reverb_zones_vector;
	
	std::vector <EchoZone> echo_zones_vector;
	
	//function to determine if effect zone should be applied or not
	void ApplyEffectZoneBasedOnListenerPosition(EffectZone* thisZone);
	
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
	
	
};



#endif
