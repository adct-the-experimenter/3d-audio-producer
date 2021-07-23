#include "SaveSystem.h"

SaveSystem::SaveSystem()
{
	
}

SaveSystem::~SaveSystem()
{
	
}

void SaveSystem::SaveProjectToSetFile(std::vector < std::unique_ptr <SoundProducer> > *sound_producer_vector_ptr,
									  EffectsManager* effectsManagerPtr,
									  Listener* listener_ptr,
									  SoundBank* sound_bank_ptr )
{
	SaveSystem::SaveProject(sound_producer_vector_ptr,effectsManagerPtr,listener_ptr,sound_bank_ptr,m_saveFilePath);
}
	
void SaveSystem::SaveAsNewProject(std::vector < std::unique_ptr <SoundProducer> > *sound_producer_vector_ptr,
									EffectsManager* effectsManagerPtr,
									Listener* listener_ptr,
									SoundBank* sound_bank_ptr,
									std::string path)
{
	m_saveFilePath = path;
	SaveSystem::SaveProject(sound_producer_vector_ptr,effectsManagerPtr,listener_ptr,sound_bank_ptr,m_saveFilePath);
}
	
void SaveSystem::SaveProject(std::vector < std::unique_ptr <SoundProducer> > *sound_producer_vector_ptr,
							 EffectsManager* effectsManagerPtr,
							 Listener* listener_ptr,
							 SoundBank* sound_bank_ptr,  
							 std::string path)
{
	
	//for each specific effect zone type
	xml_creator.SaveDataToXMLFile(sound_producer_vector_ptr,
								&effectsManagerPtr->echo_zones_vector,
								&effectsManagerPtr->standard_reverb_zones_vector,
								&effectsManagerPtr->eax_reverb_zones_vector,
								listener_ptr,
								sound_bank_ptr,
								path);
}


void SaveSystem::SetSaveFilePath(std::string path){m_saveFilePath = path;}
