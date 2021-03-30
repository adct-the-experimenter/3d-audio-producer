#include "sound_bank.h"

SoundBank::SoundBank()
{
	//for all sound accounts, initialize account number
	for(size_t i = 0; i < m_sound_accounts.size(); i++)
	{
		m_sound_accounts[i].account_number = i;
	}
	
	last_account_num = 0;
}

void SoundBank::DrawGui_Item()
{
	//for each account from start to last account number
	if(last_account_num > 0)
	{
		for(std::uint8_t i = 0; i < m_sound_accounts.size(); i++)
		{
			//draw textbox with name of sound and account number
			
			//draw open file button
			
		}
	}
	
}

void SoundBank::ChangeSoundNameForAccount(std::uint8_t account_num,std::string new_name)
{
	m_sound_accounts[account_num].name = new_name;
	account_look_up[account_num] = new_name;
}
	
void SoundBank::AddNewAccount()
{
	last_account_num++;
}
	
//used for querying sound
std::array <std::string,10> *SoundBank::GetAccountLookupTable(){return & account_look_up;}

ALuint& SoundBank::GetBufferFromThisAccount(std::uint8_t account_num){return m_sound_accounts[account_num].buffer;}
