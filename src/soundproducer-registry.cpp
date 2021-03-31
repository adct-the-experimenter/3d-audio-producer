#include "soundproducer-registry.h"


void SoundProducerRegistry::SetReferenceToSoundProducerVector(std::vector <std::unique_ptr <SoundProducer>> *sound_producer_vector)
{
	sound_producer_vector_ref = sound_producer_vector;
}

SoundProducer* SoundProducerRegistry::GetPointerToSoundProducerWithThisName(std::string thisName)
{
	//get iterator to sound producers vector from soundproducers map
	//std::unordered_map<std::string,std::vector <std::unique_ptr <SoundProducer> >::iterator>::const_iterator got = map_soundproducer.find (thisName);
	std::unordered_map<std::string,size_t>::const_iterator got = map_soundproducer.find (thisName);

	//std::vector <std::unique_ptr <SoundProducer> >::iterator it = got->second;
	size_t index = got->second;

	//return it->get();
	return sound_producer_vector_ref->at(index).get();
}

std::vector <std::string>& SoundProducerRegistry::GetSoundProducersToEditList(){return soundproducers_to_edit_string;}

void SoundProducerRegistry::AddRecentSoundProducerMadeToRegistry()
{
	if(sound_producer_vector_ref != nullptr)
	{
		//std::cout << "Add recent sound producer made to track called! \n";


		SoundProducer* thisSoundProducer = nullptr;
		//std::vector <std::unique_ptr <SoundProducer> >::iterator it;
		size_t index = 0;

		if(sound_producer_vector_ref->size() >= 1)
		{
			thisSoundProducer = sound_producer_vector_ref->at(sound_producer_vector_ref->size() - 1).get();
			//it = sound_producer_vector_ref->end() - 1;
			index = sound_producer_vector_ref->size() - 1;

			if(thisSoundProducer != nullptr)
			{
				std::string thisString = thisSoundProducer->GetNameString();

				soundproducers_to_edit_string.push_back(thisString);

				map_soundproducer.emplace(thisSoundProducer->GetNameString(),index);
			}
		}


	}

}

void SoundProducerRegistry::RemoveSoundProducerFromRegistry(SoundProducer* thisSoundProducer)
{
	std::string thisString = thisSoundProducer->GetNameString();
	
	//soundproducers_to_edit_string.Remove(thisString);

	//get index of sound producer to remove
	size_t index_to_rm  = 0;
	std::unordered_map<std::string,size_t>::const_iterator got = map_soundproducer.find (thisSoundProducer->GetNameString());
	index_to_rm = got->second;

	//decrement index of all soundproducers after this index
	for ( auto it = map_soundproducer.begin(); it != map_soundproducer.end(); ++it )
	{
		if(it->second > index_to_rm)
		{
			it->second--;
		}
	}

	//erase sound producer from map
	map_soundproducer.erase(thisSoundProducer->GetNameString());

}


void SoundProducerRegistry::RemoveThisNameFromEditingList(std::string thisName)
{
	//soundproducers_to_edit_string.Remove(thisName);
}


void SoundProducerRegistry::AddThisNameToEditingList(std::string thisName)
{
	//soundproducers_to_edit_string.Add(thisName);
}

void SoundProducerRegistry::AddAllSoundProducersToEditingList()
{
	if(sound_producer_vector_ref != nullptr)
	{
		soundproducers_to_edit_string.clear();
		for(size_t i=0; i < sound_producer_vector_ref->size(); i++)
		{
			std::string thisString = sound_producer_vector_ref->at(i)->GetNameString();

			soundproducers_to_edit_string.push_back(thisString);
		}
	}

}


void SoundProducerRegistry::ClearAllSoundproducerNames()
{
	soundproducers_to_edit_string.clear();	
}

void SoundProducerRegistry::ClearAll()
{
	soundproducers_to_edit_string.clear();
}

void SoundProducerRegistry::AddSourceOfLastSoundProducerToSoundProducerRegistry()
{
	if(sound_producer_vector_ref != nullptr)
	{
		ALuint* thisSource = sound_producer_vector_ref->at(sound_producer_vector_ref->size() - 1)->getSource();
		sound_producer_sources_vec.push_back(thisSource);
	}
	
}
void SoundProducerRegistry::RemoveSourceOfLastTrackFromSoundProducerTrackManager()
{
	sound_producer_sources_vec.pop_back();
}
