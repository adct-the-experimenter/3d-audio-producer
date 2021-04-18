#ifndef ECHO_ZONE_H
#define ECHO_ZONE_H

#include "effect-zone.h"

struct EchoZoneProperties
{
	//AL_ECHO_DELAY
	float flDelay;
	//AL_ECHO_LRDELAY
	float flLRDelay;
	//AL_ECHO_DAMPING
	float flDamping;
	//AL_ECHO_FEEDBACK
	float flFeedback;
	//AL_ECHO_SPREAD
	float flSpread;
};

struct EchoZoneSaveData
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
	
	EchoZoneProperties properties;
	
};

class EchoZone : public EffectZone
{
public:
	EchoZone();
	~EchoZone();
	
	
	//functions to initialize echo zones
	
	void InitEchoZone(std::string& thisName,
							float& x, float& y, float& z, float& width,
							EchoZoneProperties& properties);
							
	void InitEchoZoneWithGraphicalObject(std::string& thisName,
							float& x, float& y, float& z, float& width,
							EchoZoneProperties& properties);

	//changed from effect zone to implement save data
	void SetPositionX(float& x); 
	void SetPositionY(float& y); 
	void SetPositionZ(float& z); 
	void ChangeWidth(float width);
	
	//function to change reverb properties 
	void ChangeEchoZoneProperties(EchoZoneProperties& properties);
	
	EchoZoneSaveData GetEchoZoneSaveData();
	
	EchoZoneProperties& GetEchoZoneProperties();
	
	virtual ALuint* GetEffectPointer();
	virtual ALuint* GetEffectsSlotPointer();
	
	virtual ALuint GetEffect();
	virtual ALuint GetEffectsSlot();
	
	virtual void FreeEffects();
	
private:

	ALuint m_effect;
	ALuint m_slot;
	
	ALuint LoadStandardReverbEffect(const EFXEAXREVERBPROPERTIES *reverb);
	ALuint LoadEAXReverbEffect(const EFXEAXREVERBPROPERTIES *reverb);
	
	EchoZoneProperties m_echo_prop;
	
	Color echoZoneColor;
	
	EchoZoneSaveData m_saveData;
	
};

#endif
