#include "sound_bank.h"

#include "audio-stream-container.h"
#include <sndfile.h>

///Data Directory File Path

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

//DATADIR macro used by automake for path to data files
//if DATADIR macro is not defined, define it as data
#ifndef DATADIR
    #define DATADIR "../data/resources"
#endif

#define DATADIR_NAME STR(DATADIR)

#include <unistd.h>

#include "raygui/raygui.h"


#include "raygui/gui_file_dialog.h"

#include <cstring>

#include "dialog_var.h"

#include "imgui.h"
#include "backends/rlImGui.h"
#include "backends/imfilebrowser.h"

//static GuiFileDialogState fileDialogState;

//for .flac audio file decoding
#define DR_FLAC_IMPLEMENTATION
#include "backends/dr_flac.h"

//for .wav file decoding
#define DR_WAV_IMPLEMENTATION
#include "backends/dr_wav.h"

#include <fstream>

SoundBank::SoundBank()
{
	SoundBank::InitDataDirectory("");	
}

struct TextBoxParam
{
	char char_name[20] = " ";
	bool name_box_pressed = false;
};

std::array <TextBoxParam,10> name_textboxes;

std::array <std::string,10> filepath_textboxes;
char fileNameToLoad[512] = { 0 };
std::uint8_t current_file_button_edit = 0;

static bool Open = true;

static ImGui::FileBrowser sound_fileDialog_loader(0);

bool IsFileWav(std::string filepath)
{
	if(filepath.substr(filepath.size() - 4, 4) == ".wav" )
	{
		return true;
	}
	
	return false;
}

bool IsFileFLAC(std::string filepath)
{
	if(filepath.substr(filepath.size() - 5, 5) == ".flac" )
	{
		return true;
	}
	
	return false;
}

void SoundBank::DrawGui_Item()
{
	//draw rectangle

	
	if (ImGui::Begin("Sound Bank", &Open, ImGuiWindowFlags_AlwaysVerticalScrollbar))
	{
		
		soundbank_menu_in_use = ImGui::IsWindowHovered();
		
		ImGui::Text("Name \t Account Number \t File");
		
		//for each account from start to last account number
		for(std::uint8_t i = 0; i < m_sound_accounts.size(); i++)
		{
			
			//draw textbox with name of sound and account number
			std::string label_str = std::to_string(i); //account number
			if(ImGui::InputText(label_str.c_str(), name_textboxes[i].char_name, 32))
			{
				//if text box input is changed
				name_textboxes[i].name_box_pressed = !name_textboxes[i].name_box_pressed;
				SoundBank::ChangeSoundNameForAccount( i , std::string(name_textboxes[i].char_name) );
			}
			
			ImGui::SameLine();
			
			std::string file_button_label = "f" + std::to_string(i);
			if( ImGui::SmallButton(file_button_label.c_str()) )
			{
				//call file dialog
				current_file_button_edit = i; //set index of account being edited
				
				sound_fileDialog_loader.SetTitle("Open sound file.");
				sound_fileDialog_loader.SetTypeFilters({ ".wav", ".flac" });
				sound_fileDialog_loader.Open();
				global_dialog_in_use = true;
			}
						
		}
		
		//if file dialog has a file selected
		if (sound_fileDialog_loader.HasSelected())
		{
			std::string filepath = sound_fileDialog_loader.GetSelected().string();
			
			// Load sound file (if supported extension)
			if (IsFileWav(filepath) || IsFileFLAC(filepath))
			{
				//std::cout << "filepath in main gui editor: " << filepath << std::endl;
				//load audio data
				SoundBank::LoadAudioDataFromFileToAccount(filepath,current_file_button_edit);
			}

			sound_fileDialog_loader.ClearSelected();
			global_dialog_in_use = false;
		}
		
		sound_fileDialog_loader.Display();
	}
		
	ImGui::End();
	
}

void SoundBank::ChangeSoundNameForAccount(std::uint8_t account_num,std::string new_name)
{
	m_sound_accounts[account_num].name = new_name;
	account_look_up[account_num] = new_name;
	
	m_sound_bank_save_data.sound_account_data[account_num].name = new_name;
}
	
//used for querying sound
std::array <std::string,10> &SoundBank::GetAccountLookupTable(){return account_look_up;}

#define	BUFFER_LEN	1024
#define	MAX_CHANNELS	2

bool ReadAndCopyDataFromInputFile_WAV(AudioData* audio_data_ptr,std::string inputSoundFilePath)
{
	//The code taken from example code in dr_wav.h uses a version of the API that 
	//converts the audio data to a consistent format 
	//(32-bit signed PCM, in this case)

	//read data
	
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalPCMFrameCount;
    
    float* pSampleData = drwav_open_file_and_read_pcm_frames_f32(inputSoundFilePath.c_str(), &channels, &sampleRate, &totalPCMFrameCount, NULL);
    if (pSampleData == NULL) {
        // Error opening and reading WAV file.
        std::cout << "Error! Could not open input file " << inputSoundFilePath << " in sound bank.\n"; 
        return false;
    }

    audio_data_ptr->channels = channels;
    audio_data_ptr->sampleRate = sampleRate;
    audio_data_ptr->total_frames = totalPCMFrameCount;
    
    //copy audio samples to vector
    audio_data_ptr->audio_samples.resize(audio_data_ptr->total_frames);
    for(drwav_uint64 i = 0; i < totalPCMFrameCount; i++)
    {
		audio_data_ptr->audio_samples[i] = pSampleData[i];
	}

    drwav_free(pSampleData, NULL);
	
	return true;
}

bool ReadAndCopyDataFromInputFile_FLAC(AudioData* audio_data_ptr,std::string inputSoundFilePath)
{
	
	unsigned int channels;
    unsigned int sampleRate;
    drflac_uint64 totalPCMFrameCount;
    
    //decode audio data
    float* pSampleData = drflac_open_file_and_read_pcm_frames_f32(inputSoundFilePath.c_str(), &channels, &sampleRate, &totalPCMFrameCount, NULL);
    if (pSampleData == NULL) {
        // Failed to open and decode FLAC file.
        return false;
    }
    
    audio_data_ptr->channels = channels;
    audio_data_ptr->sampleRate = sampleRate;
    audio_data_ptr->total_frames = totalPCMFrameCount;
    
    

    //copy audio samples to vector
    audio_data_ptr->audio_samples.resize(audio_data_ptr->total_frames);
    for(drflac_uint64 i = 0; i < totalPCMFrameCount; i++)
    {
		audio_data_ptr->audio_samples[i] = pSampleData[i];
	}

    drflac_free(pSampleData, NULL);
    
    return true;
}

bool WriteAudioDataToWAVFile(AudioData* audio_data_ptr,std::string outputSoundFilePath){
	
	//initialize wav format
	drwav_data_format format;
    format.container = drwav_container_riff;     // <-- drwav_container_riff = normal WAV files, drwav_container_w64 = Sony Wave64.
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT; // IEEE FLOAT 32-bit
    format.channels = audio_data_ptr->channels;
    format.sampleRate = audio_data_ptr->sampleRate;
    format.bitsPerSample = 32; //32 bit
       
    
	// fstream is Stream class to both
	std::fstream file;

	// opening output file, creating it if it doesn't exist, overwriting file if it does.
	// in out(write) and truncate mode
	// ios::out Open for output operations.
	file.open(outputSoundFilePath.c_str(),std::fstream::out | std::fstream::trunc);

	// If no file is created, then
	// show the error message.
	if(!file)
	{
	   std::cout << "Error in creating file!!!\n";
	   return false;
	}

	// closing the file.
	file.close();
    
    //initialize output file
    drwav wav;
    
    if (!drwav_init_file_write(&wav, outputSoundFilePath.c_str(), &format, NULL)) {
        printf("Failed to open file.\n");
        return false;
    }
    
	
	//write to output file
	drflac_int32* pSampleData = (drflac_int32*)audio_data_ptr->audio_samples.data();
	uint64_t sample_count = audio_data_ptr->total_frames;
    
    drwav_write_pcm_frames(&wav, sample_count, pSampleData);
    
    drwav_uninit(&wav);
    
    return true;
}


void SoundBank::LoadAudioDataFromFileToAccount(std::string filepath,std::uint8_t account_num)
{
	//Hold data for left channel and right channel
	AudioData audio_data;
		
	std::cout << "Input Sound file path:" << filepath << std::endl;
	
	std::cout << "Stream sound file path: " << m_sound_accounts[account_num].stream_file_path << std::endl;
	
	bool readDone = false;
	bool writeDone = false;
		
	//if file type is .wav
	if(filepath.substr(filepath.size() - 4, 4) == ".wav")
	{
		readDone = ReadAndCopyDataFromInputFile_WAV(&audio_data,filepath);
	}
	//else if file type is .flac
	else if(filepath.substr(filepath.size() - 5, 5) == ".flac")
	{
		readDone = ReadAndCopyDataFromInputFile_FLAC(&audio_data,filepath);
	}
	//else unsupported file type.
	else{
		std::cout << "Error unsupported file type in audio file " << filepath << 
		". Supported filetypes are .wav , .flac \n";
	}
	
	writeDone = WriteAudioDataToWAVFile(&audio_data,m_sound_accounts[account_num].stream_file_path);
										
	m_sound_accounts[account_num].active = readDone && writeDone;
	
	//clear data stored
	audio_data.audio_samples.clear();
}

void SoundBank::LoadSaveData(SoundBankSaveData& data)
{
	m_sound_bank_save_data = data;
	
	//set sound account data from save data
	//set account lookup from save data
	for(size_t i = 0; i < m_sound_accounts.size(); i++)
	{
		m_sound_accounts[i] = data.sound_account_data[i];
		
		#if defined(WIN32)
		m_sound_accounts[i].stream_file_path = m_data_dir_path  + "stream-file" + std::to_string(i) + ".wav";
		#else
		m_sound_accounts[i].stream_file_path = m_data_dir_path + "stream-file" + std::to_string(i) + ".wav";
		#endif
		
		account_look_up[i] = data.sound_account_data[i].name;
		
		SoundBank::ChangeSoundNameForAccount( i ,  data.sound_account_data[i].name);
		
		strncpy(name_textboxes[i].char_name,m_sound_accounts[i].name.c_str(),20);
		name_textboxes[i].char_name[19] = '\0';
		
		
		//filepath_textboxes[i] = data.sound_account_data[i].stream_file_path;
		
		if(data.sound_account_data[i].active)
		{
			if(!data.sound_account_data[i].stream_file_path.empty())
			{
				SoundBank::LoadAudioDataFromFileToAccount(m_sound_accounts[i].stream_file_path, i);
			}
		}
				
	}
	
}

SoundBankSaveData& SoundBank::GetSaveData(){return m_sound_bank_save_data;}

void SoundBank::InitDataDirectory(std::string filepath)
{
	m_data_dir_path = filepath;
	
	//for all sound accounts, initialize account number and stream file path
	for(size_t i = 0; i < m_sound_accounts.size(); i++)
	{
		m_sound_accounts[i].account_number = i;
				
		std::string filepath_stream = m_data_dir_path + "stream-file" + std::to_string(i) + ".wav";
		
		m_sound_bank_save_data.sound_account_data[i] = m_sound_accounts[i];
		m_sound_accounts[i].stream_file_path = filepath_stream;
		m_sound_accounts[i].active = false;
	}
}
