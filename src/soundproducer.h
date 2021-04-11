#ifndef SOUNDPRODUCER_H
#define SOUNDPRODUCER_H

#include "AL/al.h" //header for OpenAL Soft
#include "AL/alc.h" //header for OpenAL Soft
#include "AL/alext.h" //header for OpenAL Soft

#include <cassert>
#include <string>
#include <cstdint>
#include <vector>
#include <array>
#include <iostream>

#include "raylib.h"

//This is a class that holds positional info on object
//as well as source and buffer components for use with sound producer track

struct SoundProducerSaveData
{
	//name
	std::string name;
	
	//position
	double x;
	double y;
	double z;
	
	//account number from sound bank
	std::uint8_t account_number;
};


class SoundProducer
{
public:
	SoundProducer();
	~SoundProducer();

	void SetNameString(std::string& thisName);
	std::string GetNameString();

	void SetPositionX(double& x); //set x position of sound producer
	double GetPositionX(); //get x position of sound producer
	void SetPositionY(double& y); //set y position of sound producer
	double GetPositionY(); //get y position of sound producer
	void SetPositionZ(double& z); //set z position of sound producer
	double GetPositionZ(); //get z position of sound producer

	void InitSoundProducer(std::string& thisName,
							double& x, double& y, double& z);

	//function to use openal soft audio engine to create source from buffer
	void CreateSource();

	void setSource(ALuint& thisSource);
	ALuint* getSource();

	void SetReferenceToTrackSource(ALuint* thisSource);
	
	SoundProducerSaveData GetSoundProducerSaveData();
	void LoadSoundProducerSaveData(SoundProducerSaveData& data);

	void SetFreeRoamBool(bool state);
	bool GetFreeRoamBool();
	
	void SetAccountNumber(std::uint8_t num);
	std::uint8_t GetAccountNumber();
	
	void DrawModel();
	
	void SetPickedBool(bool state);
	
private:
	//Name of Sound Producer
	std::string name;

	//position of Sound Producer
	Vector3 producer_position;


	//source to play buffer
	ALuint m_source;

	ALuint* track_source_ptr;

	void moveSource(); //function to move source to producer position vector coordinates
	

	//save data
	SoundProducerSaveData m_saveData;

	//option for allowing sound producer to be moved by keys
	bool freeRoam;
	
	//account number from sound bank. 
	//Tells sound player which file to buffer to source of sound producer
	std::uint8_t account_number;
	
	//bool to indicate if sound producer has been clicked on for selection
	//used to draw cube a different color if picked.
	bool picked_for_selection;

};

#endif
