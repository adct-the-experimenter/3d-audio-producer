#ifndef SOUND_PRODUCER_REGISTRY
#define SOUND_PRODUCER_REGISTRY

#include "soundproducer.h"
#include <memory>
#include <unordered_map>

#include <vector>

//class to contain map of sound producer pointers and their keys

class SoundProducerRegistry
{
public:

	void SetReferenceToSoundProducerVector(std::vector <std::unique_ptr <SoundProducer>> *sound_producer_vector);
	
	SoundProducer* GetPointerToSoundProducerWithThisName(std::string thisName);
	
	//Add or remove sound producer from registry
	void AddRecentSoundProducerMadeToRegistry();
	void RemoveSoundProducerFromRegistry(SoundProducer* thisSoundProducer);
	
	//function to return list of sound producers to edit
	std::vector <std::string>& GetSoundProducersToEditList();
	
	//function to remove a name from the list of sound producers to edit
	//use if a name is already being edited by a track
	void RemoveThisNameFromEditingList(std::string thisName);
	
	//function to add a name from the list of sound producers to edit
	//use if a name was removed for editing because it was being edited and then it stopped being edited
	void AddThisNameToEditingList(std::string thisName);
	
	//function to add all names of soundproducers in map to editor list
	//use with RemoveThisNameFromEditingList to get back a soundproducer to editing list after it is not edited in soundproducer track
	void AddAllSoundProducersToEditingList(); 

	//function to clear all names from registry
	void ClearAllSoundproducerNames();
	
	//function to clear all
	void ClearAll();
	
	//function to handle adding and removing sources in sound producer registry
	void AddSourceOfLastSoundProducerToSoundProducerRegistry();
	
	void RemoveSourceOfLastTrackFromSoundProducerTrackManager();
	
	void RemoveThisSourceFromSoundProducerRegistry(int& index);
	
	friend class ImmediateModeSoundPlayer;
	friend class EffectsManager;
	
private:

	//list of names for combo box
	std::vector <std::string> soundproducers_to_edit_string;
	
	//pointer to vector of sound producers to edit
	std::vector <std::unique_ptr <SoundProducer> > *sound_producer_vector_ref; 
	
	//vector of pointers to combo boxes
	//used to update all combo boxes
	//std::vector <wxComboBox*> combo_box_ptr_vec;
	
	//dictionary to keep track of which string associated with index
	std::unordered_map <std::string, size_t> map_soundproducer; 
	
	std::vector <ALuint*> sound_producer_sources_vec;
};

#endif
