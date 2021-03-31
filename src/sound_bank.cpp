#include "sound_bank.h"

#include "audio-stream-container.h"
#include <sndfile.h>

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)


#ifndef DATADIR
    #define DATADIR "../src/timeline-track-editor/resources/"
#endif

#define DATADIR_NAME STR(DATADIR)

std::string DATADIR_STR = DATADIR_NAME;

#include <unistd.h>

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again
#include "raygui/raygui.h"



SoundBank::SoundBank()
{
	//for all sound accounts, initialize account number and stream file path
	for(size_t i = 0; i < m_sound_accounts.size(); i++)
	{
		m_sound_accounts[i].account_number = i;
		
		std::string datadir; 
	
		datadir = DATADIR_STR;
		
		#ifdef WIN32
		datadir = "../src/timeline-track-editor/resources/";
		#endif
		
		std::string filepath_stream = datadir + "stream-file" + std::to_string(i) + ".wav";
		m_sound_accounts[i].stream_file_path = filepath_stream;
	}
	
}

struct TextBoxParam
{
	char char_name[20] = " ";
	bool name_box_pressed = false;
};

std::array <TextBoxParam,10> name_textboxes;

std::array <std::string,10> filepath_textboxes;


void SoundBank::DrawGui_Item()
{
	//draw rectangle
	
	//for each account from start to last account number
	for(std::uint8_t i = 0; i < m_sound_accounts.size(); i++)
	{
		//draw account number
		char num = *(std::to_string(i).c_str());
		GuiTextBox((Rectangle){625,100 + i*30,20,25}, &num, 20, false);
		//draw textbox with name of sound and account number
		if( GuiTextBox((Rectangle){650,100 + i*30,100,25}, 
						name_textboxes[i].char_name, 20, name_textboxes[i].name_box_pressed
					) )
		{
			name_textboxes[i].name_box_pressed = !name_textboxes[i].name_box_pressed;
			SoundBank::ChangeSoundNameForAccount( i , std::string(name_textboxes[i].char_name) );
		}
		
		//draw open file button
		if( GuiButton( (Rectangle){ 750,100 + i*30,50,25 }, filepath_textboxes[i].c_str() ) )
		{
			
		}
		
	}
	
}

void SoundBank::ChangeSoundNameForAccount(std::uint8_t account_num,std::string new_name)
{
	m_sound_accounts[account_num].name = new_name;
	account_look_up[account_num] = new_name;
}
	
//used for querying sound
std::array <std::string,10> *SoundBank::GetAccountLookupTable(){return & account_look_up;}

#define	BUFFER_LEN	1024
#define	MAX_CHANNELS	2

void ReadAndCopyDataFromInputFile(std::vector<double> *audio_data_input_copy_ptr,std::string inputSoundFilePath,SF_INFO& input_sfinfo)
{
	SNDFILE *inputFile;
	
	//Read data from file
	if (! (inputFile = sf_open (inputSoundFilePath.c_str(), SFM_READ, &input_sfinfo)))
	{	
		// Open failed, so print an error message.
		std::cout << "Not able to open input file " <<  inputSoundFilePath << std::endl;
		/* Print the error message from libsndfile. */
		puts (sf_strerror (NULL)) ;
		return;
	} 
		
	if (input_sfinfo.channels > MAX_CHANNELS)
	{
		std::cout << "Not able to process more than" <<  MAX_CHANNELS << "channels.\n";
		return;
	}
	std::cout << "Successfully loaded " << inputSoundFilePath << " saving data..." << std::endl;
	
	//read input data
	std::vector<double> read_buf(BUFFER_LEN);
	size_t read_size = 0;
	
	
	while( (read_size = sf_read_double(inputFile, read_buf.data(), read_buf.size()) ) != 0)
	{
		audio_data_input_copy_ptr->insert(audio_data_input_copy_ptr->end(), read_buf.begin(), read_buf.begin() + read_size);
	}
	
	/* Close input and stream files. */
	sf_close(inputFile);
}

void CopyInputDataIntoAudioDataStream(std::vector<double> *audio_data_input_copy_ptr, AudioStreamContainer* audio_data_stream_ptr,std::string streamSoundFilePath,SF_INFO& input_sfinfo)
{
	
	//copy input audio data references to audio data stream
	audio_data_stream_ptr->ResizeAudioStream(audio_data_input_copy_ptr->size());
	for(size_t i=0; i < audio_data_stream_ptr->GetSize(); i++)
	{
		double* ref_at_i = &(audio_data_input_copy_ptr->at(i));
		audio_data_stream_ptr->SetPointerToDataAtThisSampleIndex(ref_at_i,i);
	}
	
	double slen;
	slen = audio_data_stream_ptr->GetSize() * sizeof(uint16_t);
	
	std::cout << "Size of data in bytes: " << slen << "\n";
	//if sample buffer is null or size of buffer data is zero, notify of error
	if(slen == 0)
	{
		std::string messageString;
		messageString.append("Failed to read audio from file.\n");
		return;
	}
	
	double seconds = (1.0 * input_sfinfo.frames) / input_sfinfo.samplerate ;
	std::cout << "Duration of sound:" << seconds << "s. \n";
	
	audio_data_stream_ptr->WriteStreamContentsToFile(streamSoundFilePath, input_sfinfo.format, input_sfinfo.channels, input_sfinfo.samplerate,int(BUFFER_LEN));
}

void SoundBank::LoadAudioDataFromFileToAccount(std::string filepath,std::uint8_t account_num)
{
	//object to hold audio data for streaming
	AudioStreamContainer audio_data_stream;
	
	//Hold data for left channel and right channel
	std::vector <double> audio_data_input_copy;
	
	//audio format info
	SF_INFO input_sfinfo;
		
	std::cout << "Input Sound file path:" << filepath << std::endl;
	
	std::cout << "Stream sound file path: " << m_sound_accounts[account_num].stream_file_path << std::endl;
	
	ReadAndCopyDataFromInputFile(&audio_data_input_copy,filepath,input_sfinfo);
	CopyInputDataIntoAudioDataStream(&audio_data_input_copy,&audio_data_stream, 
									m_sound_accounts[account_num].stream_file_path,
									input_sfinfo);
	
	
	//clear data stored
	audio_data_stream.ClearStreamDataStored();
	audio_data_input_copy.clear();
}


