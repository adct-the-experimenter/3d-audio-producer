#ifndef XML_READER_H
#define XML_READER_H

#include "pugixml.hpp"
#include <iostream>

#include <string>
#include <vector>

#include "listener.h"
#include "soundproducer.h"

#include "echo-zone.h"
#include "reverb-zone.h"

#include "sound_bank.h"

#include "timeline.h"

struct LoadDataHelper
{
	std::vector <SoundProducerSaveData> *sound_producer_save_data;
	std::vector <EchoZoneSaveData> *echoZonesSaveData;
	std::vector <StandardReverbZoneSaveData> *standardRevZonesSaveData;
	std::vector <EAXReverbZoneSaveData> *eaxRevZonesSaveData;
	ListenerSaveData* listener_data_ptr;
	SoundBankSaveData* sound_bank_save_data_ptr;
	TimelineSaveData* timeline_save_data_ptr;
};

//class used to save data in binaural audio editor into a xml file

class XMLReader
{
public:
	
	XMLReader();
	~XMLReader();
	
	void LoadDataFromXMLFile(LoadDataHelper& load_data_helper,
							   std::string path);
	
private:
	
	void LoadData_SoundProducers(pugi::xml_node& root, std::vector <SoundProducerSaveData> *sound_producer_save_data);

	void LoadData_EchoZones(pugi::xml_node& root,std::vector <EchoZoneSaveData> *echoZonesSaveData);
	void LoadData_StandardRevZones(pugi::xml_node& root,std::vector <StandardReverbZoneSaveData> *standardRevZonesSaveData);
	void LoadData_EAXRevZones(pugi::xml_node& root,std::vector <EAXReverbZoneSaveData> *eaxRevZonesSaveData);
		
	void LoadData_Listener(pugi::xml_node& root, ListenerSaveData& listener_data);
	
	void LoadData_SoundBank(pugi::xml_node& root, SoundBankSaveData& sound_bank_save_data);
	
	void LoadData_Timeline(pugi::xml_node& root, TimelineSaveData& timeline_save_data);
};

#endif
