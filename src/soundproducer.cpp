#include "soundproducer.h"



SoundProducer::SoundProducer()
{

	//initialize position vector
	producer_position.x = 0;
	producer_position.y = 0;
	producer_position.z = 0;

	track_source_ptr = nullptr;
	
	freeRoam = false;
}

SoundProducer::~SoundProducer()
{
	std::cout << "Sound Producer destructor called! \n";
	if(m_source != 0)
	{
		alDeleteSources(1, &m_source);
	}

}

void SoundProducer::InitSoundProducer(std::string& thisName,
									double& x, double& y, double& z)
{
	//intialize source
	SoundProducer::CreateSource();

	name = thisName;

	//set position
	producer_position.x = x;
	producer_position.y = y;
	producer_position.z = z;

	//make box as 3d model
	
	moveSource();
	
	//init save data
	m_saveData.name = thisName;
	m_saveData.x = x;
	m_saveData.y = y;
	m_saveData.z = z;
}



void SoundProducer::SetNameString(std::string& thisName){ name = thisName;}
std::string SoundProducer::GetNameString(){ return name;}

void SoundProducer::moveSource()
{
	//if source is defined
	if(m_source != 0)
	{

		//move source
		alSource3f(m_source, AL_POSITION,
				(ALfloat)producer_position.x,
				(ALfloat)producer_position.y,
				(ALfloat)producer_position.z);
	}

	if(track_source_ptr != nullptr)
	{
		//move source
		alSource3f(*track_source_ptr, AL_POSITION,
				(ALfloat)producer_position.x,
				(ALfloat)producer_position.y,
				(ALfloat)producer_position.z);
	}
}

void SoundProducer::SetPositionX(double& x)
{
	producer_position.x = x;

	moveSource();
	
	m_saveData.x = x;
}

double SoundProducer::GetPositionX(){return producer_position.x;}

void SoundProducer::SetPositionY(double& y)
{
	producer_position.y = y;

	moveSource();
	
	m_saveData.y = y;
}

double SoundProducer::GetPositionY(){return producer_position.y;}

void SoundProducer::SetPositionZ(double& z)
{
	producer_position.z = z;

	moveSource();
	
	m_saveData.z = z;
}

double SoundProducer::GetPositionZ(){return producer_position.z;}


void SoundProducer::CreateSource()
{
	alGenSources(1, &m_source);
	alSourcei(m_source, AL_SOURCE_RELATIVE, AL_FALSE);
	assert(alGetError()==AL_NO_ERROR && "Failed to setup sound source.");
}

void SoundProducer::setSource(ALuint& thisSource){m_source = thisSource;}
ALuint* SoundProducer::getSource(){return &m_source;}


void SoundProducer::SetReferenceToTrackSource(ALuint* thisSource){track_source_ptr = thisSource;}

SoundProducerSaveData SoundProducer::GetSoundProducerSaveData(){return m_saveData;}

void SoundProducer::LoadSoundProducerSaveData(SoundProducerSaveData& data)
{
	m_saveData = data;
	
}

void SoundProducer::SetFreeRoamBool(bool state){freeRoam = state;}

bool SoundProducer::GetFreeRoamBool(){return freeRoam;}

void SoundProducer::DrawModel()
{
	DrawCube(producer_position,2.0f, 2.0f, 2.0f, RED);
}

void SoundProducer::SetAccountNumber(std::uint8_t num){account_number = num;}

std::uint8_t SoundProducer::GetAccountNumber(){return account_number;}
