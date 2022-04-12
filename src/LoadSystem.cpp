#include "LoadSystem.h"

LoadSystem::LoadSystem()
{
	
}

LoadSystem::~LoadSystem()
{
	
}
	
void LoadSystem::LoadProject(LoadDataHelper& load_data_helper,
							   std::string path
							 )
{
	//read data from xml file and push into the sound producers, sound producer tracks, 
	//and effect zones
	
	xml_reader.LoadDataFromXMLFile(load_data_helper,path);
	
}
