#ifndef XMLCREATOR_H
#define XMLCREATOR_H

#include <string>
#include <memory>

#include "listener.h"
#include "soundproducer.h"

#include "echo-zone.h"
#include "reverb-zone.h"

#include "sound_bank.h"

#include "pugixml.hpp"
#include <iostream>

//class used to save data in binaural audio editor into a xml file

class XMLCreator
{
public:
	
	XMLCreator();
	~XMLCreator();
	
	//function to save data to xml file
	void SaveDataToXMLFile(std::vector < std::unique_ptr <SoundProducer> > *sound_producer_vector_ptr,
						   std::vector <EchoZone> *echoZones,
						   std::vector <ReverbZone> *standardRevZones,
						   std::vector <ReverbZone> *eaxRevZones,
						   Listener* listener_ptr,
						   SoundBank* sound_bank_ptr, 
						   std::string path);
	
private:
		
	//function to save data from effect zones to xml file
	void SaveDataXML_EffectZones(pugi::xml_node& root,
								std::vector <EchoZone> *echoZones,
							    std::vector <ReverbZone> *standardRevZones,
							    std::vector <ReverbZone> *eaxRevZones);
	
	//function to save data from sound producers to xml file
	void SaveDataXML_SoundProducers(pugi::xml_node& root,
									std::vector < std::unique_ptr <SoundProducer> > *sound_producer_vector_ptr);
										
	//function to save data from listener to xml file
	void SaveDataXML_Listener(pugi::xml_node& root, Listener* listener_ptr);
	
	//function to save data from sound bank to xml file
	void SaveDataXML_SoundBank(pugi::xml_node& root, SoundBank* sound_bank_ptr);					
};

#endif
