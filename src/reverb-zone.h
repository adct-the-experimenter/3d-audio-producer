#ifndef REVERB_ZONE_H
#define REVERB_ZONE_H

#include "effect-zone.h"

struct ReverbEAXProperties
{
	//AL_EAXREVERB_DENSITY, 
	float flDensity;
	//AL_EAXREVERB_DIFFUSION, 
	float flDiffusion;
	//AL_EAXREVERB_GAIN, 
	float flGain;
	//AL_EAXREVERB_GAINHF, 
	float flGainHF;
	//AL_EAXREVERB_GAINLF, 
	float flGainLF;
	//AL_EAXREVERB_DECAY_TIME, 
	float flDecayTime;
	//AL_EAXREVERB_DECAY_HFRATIO, 
	float flDecayHFRatio;
	//AL_EAXREVERB_DECAY_LFRATIO, 
	float flDecayLFRatio;
	//AL_EAXREVERB_REFLECTIONS_GAIN, 
	float flReflectionsGain;
	//AL_EAXREVERB_REFLECTIONS_DELAY, 
	float flReflectionsDelay;
	//alEffectfv(effect, AL_EAXREVERB_REFLECTIONS_PAN, float flReflectionsPan;
	//AL_EAXREVERB_LATE_REVERB_GAIN, 
	float flLateReverbGain;
	//AL_EAXREVERB_LATE_REVERB_DELAY, 
	float flLateReverbDelay;
	//alEffectfv(effect, AL_EAXREVERB_LATE_REVERB_PAN, float flLateReverbPan;
	//AL_EAXREVERB_ECHO_TIME, 
	float flEchoTime;
	//AL_EAXREVERB_ECHO_DEPTH, 
	float flEchoDepth;
	//AL_EAXREVERB_MODULATION_TIME, 
	float flModulationTime;
	//AL_EAXREVERB_MODULATION_DEPTH, 
	float flModulationDepth;
	//AL_EAXREVERB_HFREFERENCE, 
	float flHFReference;
	//AL_EAXREVERB_LFREFERENCE, 
	float flLFReference;
	//AL_EAXREVERB_AIR_ABSORPTION_GAINHF, 
	float flAirAbsorptionGainHF;
	//AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, 
	float flRoomRolloffFactor;
	//AL_EAXREVERB_DECAY_HFLIMIT, 
	//int iDecayHFLimit; 
};

struct EAXReverbZoneSaveData
{
	//name
	std::string name;
	
	//position
	float x;
	float y;
	float z;
	
	//size
	float width;
	
	//properties
	ReverbEAXProperties properties;
	
};

struct ReverbStandardProperties
{
	//AL_REVERB_DENSITY, 
	float flDensity;
	//AL_REVERB_DIFFUSION, 
	float flDiffusion;
	//AL_REVERB_GAIN, 
	float flGain;
	//AL_REVERB_GAINHF, 
	float flGainHF;
	//AL_REVERB_DECAY_TIME, 
	float flDecayTime;
	//AL_REVERB_DECAY_HFRATIO, 
	float flDecayHFRatio;
	//AL_REVERB_REFLECTIONS_GAIN, 
	float flReflectionsGain;
	//AL_REVERB_REFLECTIONS_DELAY, 
	float flReflectionsDelay;
	//AL_REVERB_LATE_REVERB_GAIN, 
	float flLateReverbGain;
	//AL_REVERB_LATE_REVERB_DELAY, 
	float flLateReverbDelay;
	//AL_REVERB_AIR_ABSORPTION_GAINHF, 
	float flAirAbsorptionGainHF;
	//AL_REVERB_ROOM_ROLLOFF_FACTOR, 
	float flRoomRolloffFactor;
	//AL_REVERB_DECAY_HFLIMIT, 
	//int iDecayHFLimit;
};

struct StandardReverbZoneSaveData
{
	//name
	std::string name;
	
	//position
	float x;
	float y;
	float z;
	
	//size
	float width;
	
	//properties
	ReverbStandardProperties properties;
};

class ReverbZone : public EffectZone
{
public:
	ReverbZone();
	~ReverbZone();
	
	//type of reverb to use for zone
	enum class Type: std::int8_t {NONE=0, STANDARD, EAX};
	
	void SetType(ReverbZone::Type& type);
	ReverbZone::Type& GetType();
	
	//functions to initialize reverb zones based on type
	
	void InitStandardReverbZone(std::string& thisName,
							float& x, float& y, float& z, float& width,
							ReverbStandardProperties& properties);
							
	void InitStandardReverbZoneWithGraphicalObject(std::string& thisName,
							float& x, float& y, float& z, float& width,
							ReverbStandardProperties& properties);
							
	void InitEAXReverbZone(std::string& thisName,
							float& x, float& y, float& z, float& width,
							ReverbEAXProperties& properties);
							
	void InitEAXReverbZoneWithGraphicalObject(std::string& thisName,
							float& x, float& y, float& z, float& width,
							ReverbEAXProperties& properties);
	
							
	//function to change reverb properties 
	void ChangeStandardReverbZoneProperties(ReverbStandardProperties& properties);
	void ChangeEAXReverbZoneProperties(ReverbEAXProperties& properties);
	
	ReverbStandardProperties& GetStandardReverbZoneProperties();
	ReverbEAXProperties& GetEAXReverbZoneProperties();
	
	
	virtual ALuint* GetEffectPointer();
	virtual ALuint* GetEffectsSlotPointer();
	
	virtual ALuint GetEffect();
	virtual ALuint GetEffectsSlot();
	
	virtual void FreeEffects();
	
	StandardReverbZoneSaveData GetStandardReverbZoneSaveData();
	void LoadStandardReverbZoneSaveData(StandardReverbZoneSaveData& data);
	
	EAXReverbZoneSaveData GetEAXReverbZoneSaveData();
	void LoadEAXReverbZoneSaveData(EAXReverbZoneSaveData& data);
	
	//changed from effect zone to implement save data
	void SetPositionX(float& x); 
	void SetPositionY(float& y); 
	void SetPositionZ(float& z); 
	void ChangeWidth(float width);
	
private:

	ALuint m_effect;
	ALuint m_slot;
	
	//type of reverb zone
	ReverbZone::Type m_type;
	
	ALuint LoadStandardReverbEffect(const EFXEAXREVERBPROPERTIES *reverb);
	ALuint LoadEAXReverbEffect(const EFXEAXREVERBPROPERTIES *reverb);
	
	ReverbStandardProperties m_standard_prop;
	ReverbEAXProperties m_eax_prop;
	
	Color standardColor;
	Color eaxColor;
	
	StandardReverbZoneSaveData m_saveDataStandard;
	EAXReverbZoneSaveData m_saveDataEAX;
};

#endif
