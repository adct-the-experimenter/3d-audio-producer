

#include "echo-zone.h"

/*
 * Some code was taken from OpenAL Reverb Example
 * OpenAL Reverb Example
 *
 * Copyright (c) 2012 by Chris Robinson <chris.kcat@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
/* Effect object functions */
static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;

/* Auxiliary Effect Slot object functions */
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;


EchoZone::EchoZone() : EffectZone()
{
	m_effect = 0;
	m_slot = 0;
	
	echoZoneColor.r = 204;
	echoZoneColor.g = 102;
	echoZoneColor.b = 0;
	echoZoneColor.a = 100;
	
	EffectZone::SetColor(echoZoneColor);
}

EchoZone::~EchoZone()
{
	
}

void EchoZone::InitEchoZone(std::string& thisName,
									float& x, float& y, float& z, float& width,
									EchoZoneProperties& properties)
{
	#define LOAD_PROC(T, x)  ((x) = (T)alGetProcAddress(#x))
		LOAD_PROC(LPALGENEFFECTS, alGenEffects);
		LOAD_PROC(LPALDELETEEFFECTS, alDeleteEffects);
		LOAD_PROC(LPALISEFFECT, alIsEffect);
		LOAD_PROC(LPALEFFECTI, alEffecti);
		LOAD_PROC(LPALEFFECTIV, alEffectiv);
		LOAD_PROC(LPALEFFECTF, alEffectf);
		LOAD_PROC(LPALEFFECTFV, alEffectfv);
		LOAD_PROC(LPALGETEFFECTI, alGetEffecti);
		LOAD_PROC(LPALGETEFFECTIV, alGetEffectiv);
		LOAD_PROC(LPALGETEFFECTF, alGetEffectf);
		LOAD_PROC(LPALGETEFFECTFV, alGetEffectfv);

		LOAD_PROC(LPALGENAUXILIARYEFFECTSLOTS, alGenAuxiliaryEffectSlots);
		LOAD_PROC(LPALDELETEAUXILIARYEFFECTSLOTS, alDeleteAuxiliaryEffectSlots);
		LOAD_PROC(LPALISAUXILIARYEFFECTSLOT, alIsAuxiliaryEffectSlot);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTI, alAuxiliaryEffectSloti);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTIV, alAuxiliaryEffectSlotiv);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTF, alAuxiliaryEffectSlotf);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTFV, alAuxiliaryEffectSlotfv);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTI, alGetAuxiliaryEffectSloti);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTIV, alGetAuxiliaryEffectSlotiv);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTF, alGetAuxiliaryEffectSlotf);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTFV, alGetAuxiliaryEffectSlotfv);
	#undef LOAD_PROC
	
	/* Create the effect object. */
	alGenEffects(1, &m_effect);
	
	alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_ECHO);


	alEffectf(m_effect, AL_ECHO_DELAY, properties.flDelay);
	alEffectf(m_effect, AL_ECHO_LRDELAY, properties.flLRDelay);
	alEffectf(m_effect, AL_ECHO_DAMPING, properties.flDamping);
	alEffectf(m_effect, AL_ECHO_FEEDBACK, properties.flFeedback);
	alEffectf(m_effect, AL_ECHO_SPREAD, properties.flSpread);
	
	/* Check if an error occured, and clean up if so. */
	ALenum err = alGetError();
	if(err != AL_NO_ERROR)
	{
		if(alIsEffect(m_effect))
			alDeleteEffects(1, &m_effect);
			
		fprintf(stderr, "OpenAL error: %s\n", alGetString(err));
		return;
	}
	
	/* Create the effect slot object. This is what "plays" an effect on sources
     * that connect to it. */
    alGenAuxiliaryEffectSlots(1, &m_slot);
	EffectZone::SetEffectsSlotPointer(&m_slot);
	
    /* Tell the effect slot to use the loaded effect object. Note that the this
     * effectively copies the effect properties. You can modify or delete the
     * effect object afterward without affecting the effect slot.
     */
    ALint i_effect = (ALint)(m_effect);
    alAuxiliaryEffectSloti(m_slot, AL_EFFECTSLOT_EFFECT, i_effect);
    assert(alGetError()== AL_NO_ERROR && "Failed to set effect slot");
    
	EffectZone::InitEffectZone(thisName,x,y,z,width);
	
	//initialize standard properties
	m_echo_prop = properties;
	m_saveData.properties = properties;
	m_saveData.name = thisName;
	m_saveData.x = x;
	m_saveData.y = y;
	m_saveData.z = z;
	m_saveData.width = width;
	
}

void EchoZone::InitEchoZoneWithGraphicalObject(std::string& thisName,
							float& x, float& y, float& z, float& width,
							EchoZoneProperties& properties)
{
	
	EchoZone::InitEchoZone( thisName,
									x, y, z, width,
									properties);
	
	EffectZone::InitEffectZoneWithGraphicalObject(thisName,x,y,z,width,echoZoneColor);
}

void EchoZone::ChangeEchoZoneProperties(EchoZoneProperties& properties)
{

	/* Set new porperties */
	alEffecti(m_effect, AL_EFFECT_TYPE, AL_EFFECT_ECHO);

	alEffectf(m_effect, AL_ECHO_DELAY, properties.flDelay);
	alEffectf(m_effect, AL_ECHO_LRDELAY, properties.flLRDelay);
	alEffectf(m_effect, AL_ECHO_DAMPING, properties.flDamping);
	alEffectf(m_effect, AL_ECHO_FEEDBACK, properties.flFeedback);
	alEffectf(m_effect, AL_ECHO_SPREAD, properties.flSpread);
	
    
    m_echo_prop = properties;
    
    //load the newly modified effect into the effect slot
	
	/* Tell the effect slot to use the loaded effect object. Note that the this
     * effectively copies the effect properties. You can modify or delete the
     * effect object afterward without affecting the effect slot.
     */
    ALint i_effect = (ALint)(m_effect);
    alAuxiliaryEffectSloti(m_slot, AL_EFFECTSLOT_EFFECT, i_effect);
    assert(alGetError()== AL_NO_ERROR && "Failed to set effect slot");
    
    m_echo_prop = properties;
    m_saveData.properties = properties;
}


EchoZoneProperties& EchoZone::GetEchoZoneProperties()
{
	return m_echo_prop;
}


ALuint* EchoZone::GetEffectPointer(){ return &m_effect;}
ALuint* EchoZone::GetEffectsSlotPointer(){return &m_slot;}

ALuint EchoZone::GetEffect(){return m_effect;}
ALuint EchoZone::GetEffectsSlot(){return m_slot;}

void EchoZone::FreeEffects()
{
	//std::cout << "\nFree effects called!\n";
	ALenum err;
	
	err = alGetError();
	if(err != AL_NO_ERROR)
	{
		fprintf(stderr, "1. free effects. value above code wrong. OpenAL error: %s\n", alGetString(err));
	}
	
	if(m_effect)
	{
		alDeleteEffects(1, &m_effect);
		m_effect = 0;
	}
	
	m_effect = 0;
	
	err = alGetError();
	if(err != AL_NO_ERROR)
	{
		fprintf(stderr, "2. free effects. value above code wrong. OpenAL error: %s\n", alGetString(err));
	}
	
	if(m_slot)
	{
		 alDeleteAuxiliaryEffectSlots(1, &m_slot);
		 m_slot = 0;
	}
	
	err = alGetError();
	if(err != AL_NO_ERROR)
	{
		fprintf(stderr, "3. free effects. value above code wrong. OpenAL error: %s\n", alGetString(err));
	}
	
	m_slot = 0;
}

EchoZoneSaveData EchoZone::GetEchoZoneSaveData()
{
	return m_saveData;
}

void EchoZone::SetPositionX(float& x){m_saveData.x = x; EffectZone::SetPositionX(x);}

void EchoZone::SetPositionY(float& y){m_saveData.y = y; EffectZone::SetPositionY(y);} 

void EchoZone::SetPositionZ(float& z){m_saveData.z = z; EffectZone::SetPositionZ(z);} 

void EchoZone::ChangeWidth(float width){m_saveData.width = width; EffectZone::ChangeWidth(width);}
