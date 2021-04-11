#include "effect-zone.h"

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


EffectZone::EffectZone()
{	
	
	//initialize position vector
	zone_position = {0,0,0};
	m_width = 0;
	m_color = {245, 245, 245, 150};
}

EffectZone::~EffectZone()
{
	
}

void EffectZone::DrawModel()
{
	DrawCube(zone_position,m_width, m_width, m_width, YELLOW);
}

void EffectZone::InitEffectZone(std::string& thisName,
									double& x, double& y, double& z, double& width)
{   
	//set name
	name = thisName;
	
	//set position
	zone_position.x = x;
	zone_position.y = y;
	zone_position.z = z;
	
	//set width
	m_width = width;
	
}

void EffectZone::InitEffectZoneWithGraphicalObject(std::string& thisName,
							double& x, double& y, double& z, double& width, Color& color)
{
	
	EffectZone::InitEffectZone( thisName,x, y, z, width);
	m_color = color;
}



void EffectZone::SetNameString(std::string& thisName){ name = thisName;}
std::string EffectZone::GetNameString(){ return name;}


void EffectZone::SetPositionX(double& x)
{
	zone_position.x = x;

}

double EffectZone::GetPositionX(){return zone_position.x;}

void EffectZone::SetPositionY(double& y)
{
	zone_position.y = y;
}

double EffectZone::GetPositionY(){return zone_position.y;}

void EffectZone::SetPositionZ(double& z)
{
	zone_position.z = z;
}

double EffectZone::GetPositionZ(){return zone_position.z;}


void EffectZone::ChangeWidth(double width){m_width = width;}

double EffectZone::GetWidth(){return m_width;}

void EffectZone::SetColor(Color color)
{
	m_color = color;
}

void EffectZone::SetEffectsSlotPointer(ALuint* slot_ptr){m_slot_ptr = slot_ptr;}

ALuint* EffectZone::GetEffectPointer(){return nullptr;}
ALuint* EffectZone::GetEffectsSlotPointer(){ std::cout << "Base class called! slot ptr:" << m_slot_ptr << std::endl; return m_slot_ptr;}
	
ALuint EffectZone::GetEffect(){return 0;}
ALuint EffectZone::GetEffectsSlot(){return 0;}
	
void EffectZone::FreeEffects(){}


