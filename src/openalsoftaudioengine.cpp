#include "openalsoftaudioengine.h"


/*
 * Code adapted from OpenAL Source Play Example and HRTF example
 *
 * Copyright (c) 2017 by Chris Robinson <chris.kcat@gmail.com>
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

OpenAlSoftAudioEngine::OpenAlSoftAudioEngine()
{
	
	//Initialize sound setup variables to point to nullptr
	gAudioDevice = nullptr;
	alContext = nullptr;

    //clear testHRTF Results string
    testHRTF_Results.clear();
    loadSound_Results.clear();
}

OpenAlSoftAudioEngine::~OpenAlSoftAudioEngine()
{
    OpenAlSoftAudioEngine::close_openALSoft();
}


bool OpenAlSoftAudioEngine::initOpenALSoft()
{
	
	//use default audio device
	gAudioDevice = alcOpenDevice(NULL);
	if(gAudioDevice == NULL)
	{
		std::cout << "OpenAL Soft was unable to initialize! \n";
		std::cout << "Unable to initialize default audio device! \n";
		std::cout << alGetString(alGetError()) << std::endl;
		return false;
	}

	//create context
	alContext = alcCreateContext(gAudioDevice, NULL);
	if(alContext == NULL)
	{
		std::cout << "OpenAL Soft was unable to initialize! \n";
		std::cout << "Unable to initialize OpenAL context! \n";
		std::cout << alGetString(alGetError()) << std::endl;
		return false;
	}
	else{alcMakeContextCurrent(alContext);}

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



void OpenAlSoftAudioEngine::close_openALSoft()
{		
	alcDestroyContext(alContext);	//delete context
	alcCloseDevice(gAudioDevice);	//close device
}

ALCdevice* OpenAlSoftAudioEngine::GetReferenceToAudioDevice(){return gAudioDevice;}
ALCcontext* OpenAlSoftAudioEngine::GetReferenceToAudioContext(){return alContext;}


//HRTF
//define functions for hrtf
static LPALCGETSTRINGISOFT alcGetStringiSOFT;
static LPALCRESETDEVICESOFT alcResetDeviceSOFT;


void OpenAlSoftAudioEngine::TestHRTF()
{

    //call after initOpenALSoft

    ALCint hrtf_state; //state of hrtf, enabled or disabled
    ALCint num_hrtf; //variable for number of hrtf avaiable

    /* Check for HRTF support. */
	
		
	if(!alcIsExtensionPresent(gAudioDevice, "ALC_SOFT_HRTF"))
	{
		//fprintf(stderr, "Error: ALC_SOFT_HRTF not supported\n");
		testHRTF_Results.append("Error: ALC_SOFT_HRTF not supported! \n");
	}
	else
	{
		testHRTF_Results.append(" ALC_SOFT_HRTF is supported! \n");

		/* Define a macro to help load the function pointers. */
		#define LOAD_PROC_alcGetStringiSOFT(d, x)  ((x) = (LPALCGETSTRINGISOFT)alcGetProcAddress((d), #x))
			LOAD_PROC_alcGetStringiSOFT(gAudioDevice, alcGetStringiSOFT);
		#undef LOAD_PROC__alcGetStringiSOFT

		#define LOAD_PROC_alcResetDeviceSOFT(d, x)  ((x) = (LPALCRESETDEVICESOFT)alcGetProcAddress((d), #x))
			LOAD_PROC_alcResetDeviceSOFT(gAudioDevice, alcResetDeviceSOFT);
		#undef LOAD_PROC_alcResetDeviceSOFT

		/* Check for the AL_EXT_STEREO_ANGLES extension to be able to also rotate
		 * stereo sources.
		 */
		ALboolean has_angle_ext = alIsExtensionPresent("AL_EXT_STEREO_ANGLES");

		//printf("AL_EXT_STEREO_ANGLES%s found\n", has_angle_ext?"":" not");
		if(has_angle_ext )
		{
			testHRTF_Results.append("AL_EXT_STEREO_ANGLES support found. Stereo sources can be rotated. \n");
		}
		else
		{
			testHRTF_Results.append("AL_EXT_STEREO_ANGLES support not found. Stereo sources cannot be rotated. \n");
		}


		/* Enumerate available HRTFs, and reset the device using one. */
		alcGetIntegerv(gAudioDevice, ALC_NUM_HRTF_SPECIFIERS_SOFT, 1, &num_hrtf);
		if(!num_hrtf)
		{
			testHRTF_Results.append("No HRTFs found. \n");
		}
		else
		{
			ALCint i; //iterator

			testHRTF_Results.append("Available HRTFs:\n");
			for(i = 0;i < num_hrtf;i++)
			{
				const ALCchar *name = alcGetStringiSOFT(gAudioDevice, ALC_HRTF_SPECIFIER_SOFT, i);

				testHRTF_Results.append(name);
				testHRTF_Results.append(" ");
				testHRTF_Results.append(std::to_string(i));
				testHRTF_Results.append(" \n");
			}
		}

		/* Check if HRTF is enabled, and show which is being used. */
		alcGetIntegerv(gAudioDevice, ALC_HRTF_SOFT, 1, &hrtf_state);
		if(!hrtf_state)
		{
			testHRTF_Results.append("HRTF not enabled!\n");
		}
		else
		{
			const ALchar *name = alcGetString(gAudioDevice, ALC_HRTF_SPECIFIER_SOFT);
			
			testHRTF_Results.append("HRTF enabled, using ");
			testHRTF_Results.append(name);
			testHRTF_Results.append("\n");
		}
	}
	
}

std::string OpenAlSoftAudioEngine::getHRTFTestResult(){return testHRTF_Results;}

void OpenAlSoftAudioEngine::clear_testHRTFResults(){testHRTF_Results.clear();}

void OpenAlSoftAudioEngine::GetAvailableHRTFNames(std::vector <std::string> *names_vector)
{
	
	/* Define a macro to help load the function pointers. */
		#define LOAD_PROC_alcGetStringiSOFT(d, x)  ((x) = (LPALCGETSTRINGISOFT)alcGetProcAddress((d), #x))
			LOAD_PROC_alcGetStringiSOFT(gAudioDevice, alcGetStringiSOFT);
		#undef LOAD_PROC__alcGetStringiSOFT
		
	 ALCint num_hrtf; //variable for number of hrtf avaiable
	
	/* Enumerate available HRTFs, and reset the device using one. */
	alcGetIntegerv(gAudioDevice, ALC_NUM_HRTF_SPECIFIERS_SOFT, 1, &num_hrtf);
	if(!num_hrtf)
	{
		return;
	}
	else
	{
		ALCint i; //iterator

		for(i = 0;i < num_hrtf;i++)
		{
			const ALCchar *name = alcGetStringiSOFT(gAudioDevice, ALC_HRTF_SPECIFIER_SOFT, i);
			
			std::string name_string(name);
			names_vector->push_back(name_string);
		}
	}
}

std::string OpenAlSoftAudioEngine::GetCurrentHRTFSelected()
{
	/* Check if HRTF is enabled, and show which is being used. */
	const ALchar *name = alcGetString(gAudioDevice, ALC_HRTF_SPECIFIER_SOFT);
	
	std::string name_string(name);
	
	return name_string;
}

void OpenAlSoftAudioEngine::SelectThisHRTFByIndex(int& index,std::string& message)
{
	/* Define a macro to help load the function pointers. */
		#define LOAD_PROC_alcGetStringiSOFT(d, x)  ((x) = (LPALCGETSTRINGISOFT)alcGetProcAddress((d), #x))
			LOAD_PROC_alcGetStringiSOFT(gAudioDevice, alcGetStringiSOFT);
		#undef LOAD_PROC__alcGetStringiSOFT

		#define LOAD_PROC_alcResetDeviceSOFT(d, x)  ((x) = (LPALCRESETDEVICESOFT)alcGetProcAddress((d), #x))
			LOAD_PROC_alcResetDeviceSOFT(gAudioDevice, alcResetDeviceSOFT);
		#undef LOAD_PROC_alcResetDeviceSOFT
		
	ALCint i;
	ALCint attr[5];
	
	i = 0;
	attr[i++] = ALC_HRTF_SOFT;
	attr[i++] = ALC_TRUE;
	attr[i++] = ALC_HRTF_ID_SOFT;
    attr[i++] = index;
    attr[i] = 0;
    
    if(!alcResetDeviceSOFT(gAudioDevice, attr))
    {
		message = "Failed to reset device: \n" + std::string( alcGetString(gAudioDevice, alcGetError(gAudioDevice)) );}
    else{message = "Successfully changed HRTF and reset device.";}
}

void OpenAlSoftAudioEngine::loadSound(ALuint* buffer, const std::string& filename)
{
	//To Do.
}


std::string OpenAlSoftAudioEngine::getLoadSoundResult(){return loadSound_Results;}

void OpenAlSoftAudioEngine::clear_LoadSoundResults(){loadSound_Results.clear();}


void OpenAlSoftAudioEngine::createSource(ALuint* buffer,ALuint* source)
{
	/* Create the source to play the sound with. */
	
	//if buffer is defined
	if(*buffer != 0)
	{
		alGenSources(1, source);
		alSourcei(*source, AL_SOURCE_RELATIVE, AL_TRUE);
		alSource3f(*source, AL_POSITION, 0.0f, 0.0f, -1.0f);
		alSourcei(*source, AL_BUFFER, *buffer);
		assert(alGetError()==AL_NO_ERROR && "Failed to setup sound source.");
	}
	
}

void al_nssleep(unsigned long nsec)
{
    struct timespec ts, rem;
    ts.tv_sec = nsec / 1000000000ul;
    ts.tv_nsec = nsec % 1000000000ul;
    while(nanosleep(&ts, &rem) == -1 && errno == EINTR)
        ts = rem;
}

void OpenAlSoftAudioEngine::playSound(ALuint* source)
{
	ALenum state;
	
	/* Play the sound until it finishes. */
	
	alGetSourcei(*source, AL_SOURCE_STATE, &state);
	if (state == AL_STOPPED || state == AL_INITIAL)
	{ 
		alSourcePlay(*source);
	}
	
	
	//alSourcePlay(*source);
	//do {
	//	al_nssleep(10000000);

	//	alGetSourcei(*source, AL_SOURCE_STATE, &state);
	//} while(alGetError() == AL_NO_ERROR && state == AL_PLAYING);

}

void OpenAlSoftAudioEngine::error_check(std::string location_str)
{
	/* Check if an error occured, and clean up if so. */
	test_error_flag = alGetError();
	if(test_error_flag != AL_NO_ERROR)
	{
		std::cout << "Error found in " << location_str << ": " << alGetString(test_error_flag) << "\n";
		return;
	}	
}

