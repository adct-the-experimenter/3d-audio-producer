#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <memory>

#include "XMLCreator.h"
#include "effects-manager.h"

struct SaveSystemDataHelper
{
	std::vector < std::unique_ptr <SoundProducer> > *sound_producer_vector_ptr;
	EffectsManager* effectsManagerPtr;
	Listener* listener_ptr;
	SoundBank* sound_bank_ptr;
	TimelineSaveData* timeline_save_data_ptr;
};

class SaveSystem
{
public:
	SaveSystem();
	~SaveSystem();
	
	//function used to save project data to already established save file
	void SaveProjectToSetFile(SaveSystemDataHelper& save_system_data_helper);
	
	//function to save project data to new file
	//overwrites member save file path
	void SaveAsNewProject(SaveSystemDataHelper& save_system_data_helper,
						  std::string path);
	
	//function to set svae file path
	void SetSaveFilePath(std::string path);
	
private:
	
	//xml file writer
	XMLCreator xml_creator;
	
	std::string m_saveFilePath;
	
	void SaveProject(SaveSystemDataHelper& save_system_data_helper,  
					 std::string path);
};

#endif
