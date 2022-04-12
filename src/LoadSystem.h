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
	void LoadProject(LoadDataHelper& load_data_helper,std::string path);
	
private:
	
	//xml file reader
	XMLReader xml_reader;
	
};

#endif
