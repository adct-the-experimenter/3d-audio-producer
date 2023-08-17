#ifndef SOUND_BANK_H
#define SOUND_BANK_H

#include "AL/al.h" //header for OpenAL Soft
#include "AL/alc.h" //header for OpenAL Soft
#include "AL/alext.h" //header for OpenAL Soft

#include <array>
#include <string>
#include <cstdint>

#include <vector>

struct AudioData {
	
	unsigned int channels; //number of channels
    unsigned int sampleRate; //sample rate
    uint64_t total_frames; //total number of samples
    std::vector<int32_t> audio_samples; //the audio samples
    
};

struct SoundAccount
{
	std::string name; //name of sound
	std::uint8_t account_number; //index in sound accounts array
	std::string stream_file_path; //path to audio file
	bool active; //indicates if account is initialize and active
};

struct SoundBankSaveData
{
	std::array <SoundAccount,10> sound_account_data;
};

class SoundBank
{
public:
	SoundBank();
	
	void DrawGui_Item();
	
	//operations for handling sound accounts
	
	//change name of sound account
	void ChangeSoundNameForAccount(std::uint8_t account_num,std::string new_name);
	
	//function to load audio data to account from a file
	void LoadAudioDataFromFileToAccount(std::string filepath,std::uint8_t account_num);
	
	//used for querying sound
	std::array <std::string,10> &GetAccountLookupTable();
		
	friend class ImmediateModeSoundPlayer;
	
	//save / load data operation
	
	void LoadSaveData(SoundBankSaveData& data);
	SoundBankSaveData& GetSaveData();
	
	//function to initialize the directory where stream files are stored
	//stored in same directory as project
	void InitDataDirectory(std::string filepath);
	
private:

	std::array <SoundAccount,10> m_sound_accounts;
	
	std::array <std::string,10> account_look_up;
	
	SoundBankSaveData m_sound_bank_save_data;
	
	std::string m_data_dir_path;
};
#endif
