#include "openalsoft-player-drwav.h"

/*
 * Code Adapted from OpenAL Audio Stream Example
 * and OpenAL Source Play Example and HRTF example
 *
 * Copyright (c) 2017 by Chris Robinson <chris.kcat@gmail.com>
 * Copyright (c) 2011 by Chris Robinson <chris.kcat@gmail.com>
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

//macro for debug
//#define DEBUG_PLAYER

OpenALSoftPlayer_DRWAV::OpenALSoftPlayer_DRWAV()
{
    //inputfile_audio_samples.clear();
    bit_size = 0;
    buffer_size = 0;
    buffer_index = 0;
}

OpenALSoftPlayer_DRWAV::~OpenALSoftPlayer_DRWAV()
{
	ClosePlayerFile();

    alDeleteBuffers(NUM_BUFFERS, buffers);
    if(alGetError() != AL_NO_ERROR)
    {
		fprintf(stderr, "Failed to delete object buffer IDs\n");
	}
	
	
	buffer_size = 0;
}

void OpenALSoftPlayer_DRWAV::InitBuffersForStreaming()
{
	// Generate the buffers
    alGenBuffers(NUM_BUFFERS, buffers);
    assert(alGetError() == AL_NO_ERROR && "Could not create buffers");
}

bool OpenALSoftPlayer_DRWAV::InitOpenALSoft(ALCdevice* thisAudioDevice, ALCcontext* thisAudioContext)
{
	//use default audio device
	thisAudioDevice = alcOpenDevice(NULL);
	if(thisAudioDevice == NULL)
	{
		std::cout << "OpenAL Soft was unable to initialize! \n";
		std::cout << "Unable to initialize default audio device! \n";
		return false;
	}

	//create context
	thisAudioContext = alcCreateContext(thisAudioDevice, NULL);
	if(thisAudioContext == NULL)
	{
		std::cout << "OpenAL Soft was unable to initialize! \n";
		std::cout << "Unable to initialize OpenAL context! \n";
		return false;
	}
	else{alcMakeContextCurrent(thisAudioContext);}

	/* Check if an error occured, and clean up if so. */
	ALenum err; //variable for error flag
	err = alGetError();
	if(err != AL_NO_ERROR)
	{
		//fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
		std::cout << "OpenAL Soft was unable to initialize! \n";
		std::cout << "OpenAL Error in initializing OpenAL Soft: ";
		std::cout << alGetString(err);
		std::cout << "\n";
		return false;
	}
	
	std::cout << " Open AL Soft Initialized! \n";
	return true;
}

void OpenALSoftPlayer_DRWAV::SetReferenceToAudioDevice(ALCdevice* thisAudioDevice){audioDevicePtr = thisAudioDevice;}
void OpenALSoftPlayer_DRWAV::SetReferenceToAudioContext(ALCcontext* thisContext){alContextPtr = thisContext;}

void OpenALSoftPlayer_DRWAV::InitSource(ALuint* source)
{
	alGenSources(1, source);
    assert(alGetError() == AL_NO_ERROR && "Could not create source");

    /* Set parameters so mono sources play out the front-center speaker and
     * won't distance attenuate. */
    alSource3i(*source, AL_POSITION, 0, 0, -1);
    alSourcei(*source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcei(*source, AL_ROLLOFF_FACTOR, 0);
	assert(alGetError() == AL_NO_ERROR && "Could not set source parameters");
}

void OpenALSoftPlayer_DRWAV::CloseOpenALSoft(ALCdevice* thisAudioDevice, ALCcontext* thisAudioContext)
{
	std::cout << "Close OpenAL Soft called!\n";
	alcDestroyContext(thisAudioContext);	//delete context
	alcCloseDevice(thisAudioDevice);	//close device
}

int OpenALSoftPlayer_DRWAV::OpenPlayerFile(const char *filename)
{
	uint32_t frame_size;

	OpenALSoftPlayer_DRWAV::ClosePlayerFile();

	/* Open the file and get the first stream from it */
	unsigned int channels;
	unsigned int sampleRate;
	drwav_uint64 totalPCMFrameCount;

	//open samples and convert to float 32 bit
	float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(filename, &channels, &sampleRate, &totalPCMFrameCount, NULL);
	if (pSampleData == NULL) {
		// Error opening and reading WAV file.
		std::cout << "Error! Unable to open file" << filename << "\n";
		return 0;
	}
	
	drwav_free(pSampleData, NULL);
	
	if (!drwav_init_file_with_metadata(&inputfile_wav, filename, 0, NULL)) {
		//error opening and initializing wav object
		std::cout << "Error! Failed to initialize wav object from file " << filename << "\n";
		return 0;
	}
	
	
	 
	 #ifdef DEBUG_PLAYER
	 std::cout << "File opened successfully for streaming.\n";
	 #endif
	 
	/* Get the stream format, and figure out the OpenAL format */

	if (inputfile_wav.channels > MAX_CHANNELS)
	{	std::cout << "Not able to process more than "; 
		std::cout << std::to_string(MAX_CHANNELS); 
		std::cout << "%d channels.\n";
		return 0;
	}

	/* Get the sound format, and figure out the OpenAL format */

	//check for number of channels
	//if audio sample has only 1 channel
	if(inputfile_wav.channels == 1)
	{
		al_format = AL_FORMAT_MONO_FLOAT32;
	}
	//else if audio sample has 2 channels
	else if(inputfile_wav.channels == 2)
	{
		al_format = AL_FORMAT_STEREO_FLOAT32;
	}
	//else notify that channel count is unsupported
	else
	{
		std::cout << "Unsupported channel count. Must be 1 or 2 channels.\n";
		return 0;
	}

	sample_rate = inputfile_wav.sampleRate;


	//set audio bit size to 32-bit float
	bit_size = sizeof(float);

	frame_size = inputfile_wav.channels * bit_size;

	/* Set the buffer size, given the desired millisecond length. */
	buffer_size = (uint64_t)sample_rate * (double(BUFFER_TIME_MS))/1000 * frame_size;

	#ifdef DEBUG_PLAYER
	std::cout << "buffer size:" << buffer_size << std::endl;
	#endif

	return 1;
}

void OpenALSoftPlayer_DRWAV::ClosePlayerFile()
{
	drwav_uninit(&inputfile_wav);
}

/* Prebuffers some audio from the file, and starts playing the source */
int OpenALSoftPlayer_DRWAV::StartPlayerBuffering(ALuint* source, double& current_time)
{
	//std::cout << "In start player!\n";
	
    buffer_index = 0; //reset buffer index

    /* Rewind the source position and clear the buffer queue */
    alSourceRewind(*source);
    alSourcei(*source, AL_BUFFER, 0);
    
    //prevent program from going further if buffer size is zero
    if(buffer_size == 0){return 0;}
    
    //convert time into sample number
    uint64_t current_sample_number = current_time * inputfile_wav.sampleRate;
    
    //move file to current sample number
    //sf_seek(infile, current_sample_number, SEEK_SET);
    drwav_seek_to_pcm_frame(&inputfile_wav, current_sample_number);
	
    /* Fill the buffer queue */
    for(buffer_index = 0; buffer_index < NUM_BUFFERS;buffer_index++)
    {
		#ifdef DEBUG_PLAYER
		std::cout << "buffer index:" << buffer_index << std::endl;
		#endif
		
		//read data differently depending on bit size set in OpenPlayerFile
		
		//if data is 32-bit float
		if(bit_size == 4)
		{
			std::vector<float> data;
			data.resize(buffer_size);
			
			//read 1 chunk of data, save it to data buffer
			uint64_t pcmFrameCount_drwav = drwav_read_pcm_frames_f32(&inputfile_wav, buffer_size, data.data());

			int slen = data.size() * sizeof(float); //get size of data in bytes
			
			#ifdef DEBUG_PLAYER
			std::cout << "StartPlayerBuffering, number of samples in data buffer : " << slen << "\n";
			#endif
			
			//if sample buffer is null or size of buffer data is zero, notify of error
			if(slen == 0)
			{
				std::cout << "Failed to read anymore audio from file. Sample length is 0! \n";
				break;
			}

			// Buffer the audio data into buffer array
		
			//set buffer data
			alBufferData(buffers[buffer_index], al_format, &data.front(), slen, inputfile_wav.sampleRate);
		}
		
        
    }
    
    if(alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Error buffering for playback\n");
        return PlayerStatus::ERROR_BUFFERING_DATA;
    }
    
    /* Now queue buffer to source */
    alSourceQueueBuffers(*source, buffer_index, buffers);
    
    return PlayerStatus::GOOD_START_BUFFERING;
}

int OpenALSoftPlayer_DRWAV::StartPlayingBuffer(ALuint* source)
{
	//Start playback assume buffer queued
    alSourcePlay(*source);
    if(alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Error starting playback\n");
        return PlayerStatus::ERROR_STARTING_PLAYBACK;
    }
    
    return PlayerStatus::GOOD_PLAYING_STATUS;
}

int OpenALSoftPlayer_DRWAV::UpdatePlayerBuffer(ALuint* source,double& current_time)
{
	 //convert time into sample number
	uint64_t current_sample_number = current_time * inputfile_wav.sampleRate;
	
	//move file to current sample number
	//sf_seek(infile, current_sample_number, SEEK_SET);
	drwav_seek_to_pcm_frame(&inputfile_wav, current_sample_number);
	
	//std::cout << "In update player! \n";
    ALint processed, state;

    /* Get relevant source info */
    alGetSourcei(*source, AL_SOURCE_STATE, &state);
    alGetSourcei(*source, AL_BUFFERS_PROCESSED, &processed);
    
    if(alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Error checking source state.\n");
        return PlayerStatus::ERROR_CHECKING_SOURCE_STATE;
    }

    /* Unqueue and handle each processed buffer */
    while(processed > 0)
    {
        ALuint bufid;
        uint32_t slen;

        alSourceUnqueueBuffers(*source, 1, &bufid);
        processed--;

        /* Read the next chunk of data, refill the buffer, and queue it
         * back on the source */
         
         //read data differently depending on the bit size set in OpenPlayerFile
         switch(bit_size)
         {
			 
			 //if 32-bit float
			 case 4:
			 {
				//setup data for buffer
				std::vector<float> data;
				data.resize(buffer_size);
				
				//read 1 chunk of data, save it to data buffer
				uint64_t pcmFrameCount_drwav = drwav_read_pcm_frames_f32(&inputfile_wav, buffer_size, data.data());
				
				slen = data.size() * sizeof(float); //get size of data in bytes
				
				#ifdef DEBUG_PLAYER
				std::cout << "Number of pcm frames read" << pcmFrameCount_drwav << "\n";
				std::cout << "Size of data in bytes: " << slen << "\n";
				#endif
				
				//if sample buffer is null or size of buffer data is zero, notify of error
				if(slen == 0)
				{
					#ifdef DEBUG_PLAYER
					std::cout << "Failed to read audio from file.\n";
					#endif
					
					return PlayerStatus::FAILED_TO_READ_ANYMORE_AUDIO_FROM_FILE;
				}

				#ifdef DEBUG_PLAYER
				double seconds = (1.0 * inputfile_wav.totalPCMFrameCount) / inputfile_wav.sampleRate;
				std::cout << "Duration of sound:" << seconds << "s. \n";
				#endif
				
				if(slen > 0)
				{
					alBufferData(bufid, al_format, &data.front(), slen, inputfile_wav.sampleRate);
					alSourceQueueBuffers(*source, 1, &bufid);
				}
				if(alGetError() != AL_NO_ERROR)
				{
					fprintf(stderr, "Error buffering data\n");
					return PlayerStatus::ERROR_BUFFERING_DATA;
				}
				break;
			 }
			 
		 }
    }
    
    return PlayerStatus::GOOD_UPDATE_BUFFER_STATUS;
}

int OpenALSoftPlayer_DRWAV::PlayUpdatedPlayerBuffer(ALuint* source)
{
	ALint state;
	
	alGetSourcei(*source, AL_SOURCE_STATE, &state);
	
	/* Make sure the source hasn't underrun */
    if(state != AL_PLAYING && state != AL_PAUSED)
    {
        ALint queued;

        /* If no buffers are queued, playback is finished */
        alGetSourcei(*source, AL_BUFFERS_QUEUED, &queued);
        if(queued == 0)
        {
			return PlayerStatus::PLAYBACK_FINISHED;
		}

        alSourcePlay(*source);
        if(alGetError() != AL_NO_ERROR)
        {
            fprintf(stderr, "Error restarting playback\n");
            return PlayerStatus::ERROR_RESTARTING_PLAYBACK;
        }
    }
    
    return GOOD_PLAYING_STATUS;
}

int OpenALSoftPlayer_DRWAV::PlayMultipleUpdatedPlayerBuffers(std::vector <ALuint*> *sources_vec)
{	
	//vector containing queued status of each source
	std::vector <ALint> queued_source_vector;
	queued_source_vector.resize(sources_vec->size());
	
	//vector containing sources that are eligible to be played i.e. no underrun 
	std::vector <ALuint> sources_to_play_vec;
	
	for(size_t i = 0; i < sources_vec->size(); i++)
	{
		ALint state;
		
		//get state of source
		alGetSourcei(*(sources_vec->at(i)), AL_SOURCE_STATE, &state);
		
		/* Make sure the source hasn't underrun */
		if(state != AL_PLAYING && state != AL_PAUSED)
		{
			ALint queued;
			
			alGetSourcei(*(sources_vec->at(i)), AL_BUFFERS_QUEUED, &queued);
			queued_source_vector[i] = queued;
		}
		
	}
	
	for(size_t i = 0; i < queued_source_vector.size(); i++)
	{
		//if source not underrun 
		if(queued_source_vector[i] != 0)
		{
			//add it to vector of eligible sources to be played
			//if source is valid
			if(alIsSource( *(sources_vec->at(i)) ) == AL_TRUE )
			{
				sources_to_play_vec.push_back( *(sources_vec->at(i)) );
			}
			else
			{
				std::cout << "Invalid source name:" << sources_vec->at(i) << std::endl;
			}
		}
	}
	
	//if there are no elegible sources to play
	if(sources_to_play_vec.size() == 0)
	{
		/* If no buffers are queued for all sources, playback is finished */
		return PlayerStatus::PLAYBACK_FINISHED;
	}
	//else if there are eligible sources to play
    else
    {
		 //play sources
		ALsizei n = sources_to_play_vec.size();
		
		const ALuint *sNames = &sources_to_play_vec.at(0);
		alSourcePlayv(n, sNames);
		
		ALenum err = alGetError();
		if(err != AL_NO_ERROR)
		{
			fprintf(stderr, "Error restarting playback in PlayMultipleUpdatedPlayerBuffers.\n");
			fprintf(stderr, "OpenAL Error: %s\n", alGetString(err));
			return PlayerStatus::ERROR_RESTARTING_PLAYBACK;
		}
	}
   
    return GOOD_PLAYING_STATUS;
}


void OpenALSoftPlayer_DRWAV::PlaySource(ALuint* thisSource)
{
	alSourcePlay(*thisSource);
	if(alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Error starting playback\n");
    }
}

void OpenALSoftPlayer_DRWAV::PlayMultipleSources(std::vector <ALuint*> *sources_vec)
{
	ALsizei n = sources_vec->size();
	const ALuint *sNames = sources_vec->at(0);
	alSourcePlayv(n,sNames);
	if(alGetError() != AL_NO_ERROR)
    {
        fprintf(stderr, "Error restarting playback in PlayMultipleSources.\n");
    }
}

void OpenALSoftPlayer_DRWAV::PauseSource(ALuint* thisSource)
{
	alSourcePause(*thisSource);
}

void OpenALSoftPlayer_DRWAV::PauseMultipleSources(std::vector <ALuint*> *sources_vec)
{
	ALsizei n = sources_vec->size();
	const ALuint *sNames = sources_vec->at(0);
	alSourcePausev (n,sNames);
}


void OpenALSoftPlayer_DRWAV::RewindSource(ALuint* thisSource)
{
	alSourceRewind(*thisSource);
}

void OpenALSoftPlayer_DRWAV::RewindMultipleSources(std::vector <ALuint*> *sources_vec)
{
	ALsizei n = sources_vec->size();
	const ALuint *sNames = sources_vec->at(0);
	alSourceRewindv (n, sNames);
}
	

void OpenALSoftPlayer_DRWAV::StopSource(ALuint* thisSource)
{
	alSourceStop(*thisSource);
}

void OpenALSoftPlayer_DRWAV::StopMultipleSources(std::vector <ALuint*> *sources_vec)
{
	ALsizei n = sources_vec->size();
	const ALuint *sNames = sources_vec->at(0);
	alSourceStopv(n,sNames);
}

void OpenALSoftPlayer_DRWAV::ClearQueue(ALuint* thisSource)
{
	alSourcei(*thisSource, AL_BUFFER, 0);
}
