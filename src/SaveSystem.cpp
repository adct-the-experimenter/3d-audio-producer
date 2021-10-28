#include "SaveSystem.h"

SaveSystem::SaveSystem()
{
	
}

SaveSystem::~SaveSystem()
{
	
}

void SaveSystem::SaveProjectToSetFile(SaveSystemDataHelper& save_system_data_helper )
{
	SaveSystem::SaveProject(save_system_data_helper,m_saveFilePath);
}
	
void SaveSystem::SaveAsNewProject(SaveSystemDataHelper& save_system_data_helper,
									std::string path)
{
	m_saveFilePath = path;
	SaveSystem::SaveProject(save_system_data_helper,m_saveFilePath);
}
	
void SaveSystem::SaveProject(SaveSystemDataHelper& save_system_data_helper,  
							 std::string path)
{
	
	SaveDataHelper save_data_helper;
	save_data_helper.sound_producer_vector_ptr = save_system_data_helper.sound_producer_vector_ptr;
	save_data_helper.echoZones = &save_system_data_helper.effectsManagerPtr->echo_zones_vector;
	save_data_helper.standardRevZones = &save_system_data_helper.effectsManagerPtr->standard_reverb_zones_vector;
	save_data_helper.eaxRevZones = &save_system_data_helper.effectsManagerPtr->eax_reverb_zones_vector;
	save_data_helper.listener_ptr = save_system_data_helper.listener_ptr;
	save_data_helper.sound_bank_ptr = save_system_data_helper.sound_bank_ptr;
	
	//for each specific effect zone type
	xml_creator.SaveDataToXMLFile(save_data_helper,path);
}


void SaveSystem::SetSaveFilePath(std::string path){m_saveFilePath = path;}
