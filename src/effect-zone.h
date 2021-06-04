#ifndef EFFECT_ZONE_H
#define EFFECT_ZONE_H


#include "AL/al.h" //header for OpenAL Soft
#include "AL/alc.h" //header for OpenAL Soft
#include "AL/alext.h" //header for OpenAL Soft
#include "AL/efx.h"
#include "AL/efx-presets.h"

#include <cassert>
#include <string>
#include <cstdint>
#include <vector>
#include <iostream>

#include "raylib.h"



class EffectZone
{

public:
	EffectZone();
	~EffectZone();
	
	void DrawModel();
	
	//name of zone
	
	void SetNameString(std::string& thisName);
	std::string GetNameString();
	
	
	//Position
	
	void SetPositionX(float& x); //set x position of sound producer
	float GetPositionX(); //get x position of sound producer
	void SetPositionY(float& y); //set y position of sound producer
	float GetPositionY(); //get y position of sound producer
	void SetPositionZ(float& z); //set z position of sound producer
	float GetPositionZ(); //get z position of sound producer
	
	
	//functions to initialize effect zone
	
	void InitEffectZone(std::string& thisName,
							float& x, float& y, float& z, float& width);
							
	void InitEffectZoneWithGraphicalObject(std::string& thisName,
							float& x, float& y, float& z, float& width,
							Color& color);
	
	
	
	void ChangeWidth(float width);
	float GetWidth();
	
	void SetColor(Color color);
	
	virtual void SetPickedBool(bool state);
	
	//OpenAL Soft effects properties
	
	void SetEffectsSlotPointer(ALuint* slot_ptr);
	
	virtual ALuint* GetEffectPointer();
	virtual ALuint* GetEffectsSlotPointer();
	
	virtual ALuint GetEffect();
	virtual ALuint GetEffectsSlot();
	
	virtual void FreeEffects();
	

private:

	ALuint* m_slot_ptr;
    
	//Name of effect Zone
	std::string name;
	
	//position of effect zone
	Vector3 zone_position;
	
	float m_width;
	
	Color m_color;
	
	bool picked;
};

#endif
