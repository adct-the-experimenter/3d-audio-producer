#include "XMLReader.h"

#include <cstdlib>

XMLReader::XMLReader()
{
	
}

XMLReader::~XMLReader()
{
	
}
	
void XMLReader::LoadDataFromXMLFile(std::vector <SoundProducerSaveData> *sound_producer_save_data,
							   std::vector <EchoZoneSaveData> *echoZonesSaveData,
							   std::vector <StandardReverbZoneSaveData> *standardRevZonesSaveData,
							   std::vector <EAXReverbZoneSaveData> *eaxRevZonesSaveData,
							   ListenerSaveData& listener_data,
							   SoundBankSaveData& sound_bank_save_data,
							   std::string path)
{
	// Create empty XML document within memory
    pugi::xml_document doc;
    
    // Load XML file into memory
    // Remark: to fully read declaration entries you have to specify
    // "pugi::parse_declaration"
    pugi::xml_parse_result result = doc.load_file(path.c_str(),
												pugi::parse_default);
    if (!result)
    {
        std::cout << "Parse error: " << result.description()
            << ", character pos= " << result.offset;
        return;
    }
    
    pugi::xml_node root = doc.child("BAEXMLRoot");
    
    XMLReader::LoadData_SoundProducers(root, sound_producer_save_data);
	XMLReader::LoadData_EchoZones(root,echoZonesSaveData);
	XMLReader::LoadData_StandardRevZones(root,standardRevZonesSaveData);
	XMLReader::LoadData_EAXRevZones(root,eaxRevZonesSaveData);
	
	XMLReader::LoadData_Listener(root,listener_data);
	XMLReader::LoadData_SoundBank(root,sound_bank_save_data);
}

void XMLReader::LoadData_SoundProducers(pugi::xml_node& root, std::vector <SoundProducerSaveData> *sound_producer_save_data)
{
	pugi::xml_node spRoot = root.child("SoundProducers");
	
	
	//go through each sound producer node
	for (pugi::xml_node sp_node = spRoot.first_child(); sp_node; sp_node = sp_node.next_sibling() )
	{	
		//DungeonTile::TileType type = DungeonTile::TileType::RED;
		SoundProducerSaveData data;
		
		std::string valString; 
		
		valString = sp_node.child("Name").attribute("name").value();
		data.name = valString;
		
		valString = sp_node.child("Position").attribute("x").value();
		data.x = atof(valString.c_str());
		
		valString = sp_node.child("Position").attribute("y").value();
		data.y = atof(valString.c_str());
		
		valString = sp_node.child("Position").attribute("z").value();
		data.z = atof(valString.c_str());
		
		valString = sp_node.child("AccountNumber").attribute("num").value();
		data.account_number = atoi(valString.c_str());
		
		valString = sp_node.child("FreeRoam").attribute("status").value();
		if(valString == "true"){ data.freeRoam = true;}
		else{data.freeRoam = false;}
		
		sound_producer_save_data->push_back(data);		
	}
		
	
}


void XMLReader::LoadData_EchoZones(pugi::xml_node& root,std::vector <EchoZoneSaveData> *echoZonesSaveData)
{
	pugi::xml_node echoZonesNodeRoot = root.child("EffectZones").child("EchoZones");
			
	for(pugi::xml_node ez_node = echoZonesNodeRoot.first_child(); ez_node; ez_node = ez_node.next_sibling() )
	{
		EchoZoneSaveData data;
		
		std::string valString;
		
		valString = ez_node.child("Name").attribute("name").value();
		data.name = valString;
		
		
		valString = ez_node.child("Position").attribute("x").value();
		data.x = atof(valString.c_str());
		
		valString = ez_node.child("Position").attribute("y").value();
		data.y = atof(valString.c_str());
		
		valString = ez_node.child("Position").attribute("z").value();
		data.z = atof(valString.c_str());
		
		valString = ez_node.child("Size").attribute("width").value();
		data.width = atof(valString.c_str());
		
		valString = ez_node.child("Property").attribute("delay").value();
		data.properties.flDelay = atof(valString.c_str());
		
		valString = ez_node.child("Property").attribute("lrdelay").value();
		data.properties.flLRDelay = atof(valString.c_str());
		
		valString = ez_node.child("Property").attribute("damping").value();
		data.properties.flDamping = atof(valString.c_str());
		
		valString = ez_node.child("Property").attribute("feedback").value();
		data.properties.flFeedback = atof(valString.c_str());
		
		valString = ez_node.child("Property").attribute("spread").value();
		data.properties.flSpread = atof(valString.c_str());
		
		echoZonesSaveData->push_back(data);
	}
}

void XMLReader::LoadData_StandardRevZones(pugi::xml_node& root,std::vector <StandardReverbZoneSaveData> *standardRevZonesSaveData)
{
	pugi::xml_node standardRevZonesNodeRoot = root.child("EffectZones").child("StandardZones");
			
	for(pugi::xml_node srz_node = standardRevZonesNodeRoot.first_child(); srz_node; srz_node = srz_node.next_sibling() )
	{
		StandardReverbZoneSaveData data;
		
		std::string valString;
		
		valString = srz_node.child("Name").attribute("name").value();
		data.name = valString;
		
		
		valString = srz_node.child("Position").attribute("x").value();
		data.x = atof(valString.c_str());
		
		valString = srz_node.child("Position").attribute("y").value();
		data.y = atof(valString.c_str());
		
		valString = srz_node.child("Position").attribute("z").value();
		data.z = atof(valString.c_str());
		
		valString = srz_node.child("Size").attribute("width").value();
		data.width = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("density").value();
		data.properties.flDensity = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("diffusion").value();
		data.properties.flDiffusion = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("gain").value();
		data.properties.flGain = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("gainhf").value();
		data.properties.flGainHF = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("decaytime").value();
		data.properties.flDecayTime = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("decayhfratio").value();
		data.properties.flDecayHFRatio = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("reflectgain").value();
		data.properties.flReflectionsGain = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("reflectdelay").value();
		data.properties.flReflectionsDelay = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("latereverbgain").value();
		data.properties.flLateReverbGain = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("latereverbdelay").value();
		data.properties.flLateReverbDelay = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("airgainhf").value();
		data.properties.flAirAbsorptionGainHF = atof(valString.c_str());
		
		valString = srz_node.child("Property").attribute("roomrolloff").value();
		data.properties.flRoomRolloffFactor = atof(valString.c_str());
		
		standardRevZonesSaveData->push_back(data);
	}
}

void XMLReader::LoadData_EAXRevZones(pugi::xml_node& root,std::vector <EAXReverbZoneSaveData> *eaxRevZonesSaveData)
{
	pugi::xml_node eaxRevZonesNodeRoot = root.child("EffectZones").child("EAXZones");
			
	for(pugi::xml_node erz_node = eaxRevZonesNodeRoot.first_child(); erz_node; erz_node = erz_node.next_sibling() )
	{
		EAXReverbZoneSaveData data;
		
		std::string valString;
		
		valString = erz_node.child("Name").attribute("name").value();
		data.name = valString;
		
		
		valString = erz_node.child("Position").attribute("x").value();
		data.x = atof(valString.c_str());
		
		valString = erz_node.child("Position").attribute("y").value();
		data.y = atof(valString.c_str());
		
		valString = erz_node.child("Position").attribute("z").value();
		data.z = atof(valString.c_str());
		
		valString = erz_node.child("Size").attribute("width").value();
		data.width = atof(valString.c_str());
		
		
		valString = erz_node.child("Property").attribute("density").value();
		data.properties.flDensity = atof(valString.c_str()); 
		
		valString = erz_node.child("Property").attribute("diffusion").value();
		data.properties.flDiffusion = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("gain").value();
		data.properties.flGain = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("gainhf").value(); 
		data.properties.flGainHF = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("gainlf").value(); 
		data.properties.flGainLF = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("decaytime").value(); 
		data.properties.flDecayTime = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("decayhfratio").value(); 
		data.properties.flDecayHFRatio = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("decaylfratio").value(); 
		data.properties.flDecayLFRatio = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("reflectgain").value(); 
		data.properties.flReflectionsGain = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("reflectdelay").value(); 
		data.properties.flReflectionsDelay = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("latereverbgain").value(); 
		data.properties.flLateReverbGain = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("latereverbdelay").value(); 
		data.properties.flLateReverbDelay = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("echotime").value(); 
		data.properties.flEchoTime = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("echodepth").value(); 
		data.properties.flEchoDepth = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("modtime").value(); 
		data.properties.flModulationTime = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("moddepth").value(); 
		data.properties.flModulationDepth = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("hfref").value(); 
		data.properties.flHFReference = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("lfref").value(); 
		data.properties.flLFReference = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("airgainhf").value(); 
		data.properties.flAirAbsorptionGainHF = atof(valString.c_str());
		
		valString = erz_node.child("Property").attribute("roomrolloff").value(); 
		data.properties.flRoomRolloffFactor = atof(valString.c_str());
		
		eaxRevZonesSaveData->push_back(data);
	}
}


void XMLReader::LoadData_Listener(pugi::xml_node& root, ListenerSaveData& listener_save_data)
{
	ListenerSaveData data;
	
	pugi::xml_node listenerNodeRoot = root.child("Listener");
			
	std::string valString = "";
	bool status = false;
	
	valString = listenerNodeRoot.child("FreeRoam").attribute("status").value();
	
	if(valString == "true"){status = true;}
	else{status = false;}
	
	data.freeRoam = status;
	
	valString = "";
	valString = listenerNodeRoot.child("ExternalOrientation").attribute("status").value();
	
	if(valString == "true"){status = true;}
	else{status = false;}
	
	data.externalOrientation = status;
	
	valString = listenerNodeRoot.child("Position").attribute("x").value();
	data.x = atof(valString.c_str());
	
	valString = listenerNodeRoot.child("Position").attribute("y").value();
	data.y = atof(valString.c_str());
	
	valString = listenerNodeRoot.child("Position").attribute("z").value();
	data.z = atof(valString.c_str());
	
	listener_save_data = data;
}

void XMLReader::LoadData_SoundBank(pugi::xml_node& root, SoundBankSaveData& sound_bank_save_data)
{
	std::string valString = "";
	
	pugi::xml_node soundBankRoot = root.child("SoundBank");
	
	pugi::xml_node accountsNodeRoot = soundBankRoot.child("Accounts");
	
	size_t iterator = 0;
	
	//go through each sound producer node
	for (pugi::xml_node account_node = accountsNodeRoot.first_child(); account_node; account_node = account_node.next_sibling() )
	{
		valString = account_node.attribute("name").value(); 
		sound_bank_save_data.sound_account_data[iterator].name = valString;
		
		valString = account_node.attribute("account_num").value(); 
		sound_bank_save_data.sound_account_data[iterator].account_number = atoi(valString.c_str());
		
		valString = account_node.attribute("filepath").value(); 
		sound_bank_save_data.sound_account_data[iterator].stream_file_path = valString;
		
		iterator++;
	}
	
}
