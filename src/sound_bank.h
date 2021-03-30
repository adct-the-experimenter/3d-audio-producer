#ifndef SOUND_BANK_H
#define SOUND_BANK_H

#include "AL/al.h" //header for OpenAL Soft
#include "AL/alc.h" //header for OpenAL Soft
#include "AL/alext.h" //header for OpenAL Soft

#include <array>
#include <string>
#include <cstdint>

struct SoundAccount
{
	std::string name; //name of sound
	std::uint8_t account_number; //index in sound accounts array
	ALuint buffer; //buffer holding data
};

class SoundBank
{
public:
	SoundBank();
	
	void DrawGui_Item();
	
	//operations for handling sound accounts
	
	void ChangeSoundNameForAccount(std::uint8_t account_num,std::string new_name);
	
	void AddNewAccount();
	
	
	//used for querying sound
	std::array <std::string,10> *GetAccountLookupTable();
	
	ALuint& GetBufferFromThisAccount(std::uint8_t account_num);
	
	
private:
	std::array <SoundAccount,10> m_sound_accounts;
	
	std::array <std::string,10> account_look_up;
	
	std::uint8_t last_account_num;
};
#endif
