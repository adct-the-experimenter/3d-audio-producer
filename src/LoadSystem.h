#ifndef LOAD_SYSTEM_H
#define LOAD_SYSTEM_H

#include <string>

#include "XMLReader.h"
#include "effects-manager.h"

#include "listener.h"

class LoadSystem
{
public:
	LoadSystem();
	~LoadSystem();
	
	//function used to load project
	void LoadProject(std::vector <SoundProducerSaveData> *sound_producer_save_data,
							   std::vector <EchoZoneSaveData> *echoZonesSaveData,
							   std::vector <StandardReverbZoneSaveData> *standardRevZonesSaveData,
							   std::vector <EAXReverbZoneSaveData> *eaxRevZonesSaveData,
							   ListenerSaveData& listener_save_data,
							   SoundBankSaveData& sound_bank_save_data,
							   std::string path);
	
private:
	
	//xml file reader
	XMLReader xml_reader;
	
};

#endif
