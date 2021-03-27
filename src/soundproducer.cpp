#include "soundproducer.h"

SoundProducer::SoundProducer()
{

	//initialize buffer as empty
	m_buffer = 0;

	//initialize position vector
	producer_position_vector.resize(3);
	producer_position_vector[POSITION_INDEX::X] = 0;
	producer_position_vector[POSITION_INDEX::Y] = 0;
	producer_position_vector[POSITION_INDEX::Z] = 0;

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

	if(m_buffer != 0)
	{
		alDeleteBuffers(1, &m_buffer);
	}
}

void SoundProducer::InitSoundProducer(std::string& thisName,std::string& filepath, ALuint& buffer,
									double& x, double& y, double& z)
{
	//intialize source
	SoundProducer::CreateSource();

	name = thisName;
	m_filepath = filepath;

	if(buffer != 0)
	{
		SoundProducer::setBuffer(buffer);
	}

	//set position
	producer_position_vector[POSITION_INDEX::X] = x;
	producer_position_vector[POSITION_INDEX::Y] = y;
	producer_position_vector[POSITION_INDEX::Z] = z;

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
				(ALfloat)producer_position_vector[POSITION_INDEX::X],
				(ALfloat)producer_position_vector[POSITION_INDEX::Y],
				(ALfloat)producer_position_vector[POSITION_INDEX::Z]);
	}

	if(track_source_ptr != nullptr)
	{
		//move source
		alSource3f(*track_source_ptr, AL_POSITION,
				(ALfloat)producer_position_vector[POSITION_INDEX::X],
				(ALfloat)producer_position_vector[POSITION_INDEX::Y],
				(ALfloat)producer_position_vector[POSITION_INDEX::Z]);
	}
}

void SoundProducer::SetPositionX(double& x)
{
	producer_position_vector[POSITION_INDEX::X] = x;

	moveSource();
	
	m_saveData.x = x;
}

double SoundProducer::GetPositionX(){return producer_position_vector[POSITION_INDEX::X];}

void SoundProducer::SetPositionY(double& y)
{
	producer_position_vector[POSITION_INDEX::Y] = y;

	moveSource();
	
	m_saveData.y = y;
}

double SoundProducer::GetPositionY(){return producer_position_vector[POSITION_INDEX::Y];}

void SoundProducer::SetPositionZ(double& z)
{
	producer_position_vector[POSITION_INDEX::Z] = z;

	moveSource();
	
	m_saveData.z = z;
}

double SoundProducer::GetPositionZ(){return producer_position_vector[POSITION_INDEX::Z];}

void SoundProducer::setFilepathToSound(std::string& filepath){m_filepath = filepath;}

std::string& SoundProducer::getFilepathToSound(){return m_filepath;}

void SoundProducer::setBuffer(ALuint& thisBuffer)
{
	m_buffer = thisBuffer;

	//attach new buffer to source if source is defined
	if(m_source != 0)
	{
		alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcei(m_source, AL_BUFFER, m_buffer);
	}
}
ALuint* SoundProducer::getBuffer(){return &m_buffer;}

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
