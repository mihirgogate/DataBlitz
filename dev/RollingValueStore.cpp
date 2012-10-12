/*
 * RollingValueStore.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: mihirgogate
 */

#include "RollingValueStore.h"
#include<iostream>

RollingValueStore::RollingValueStore() {
	// TODO Auto-generated constructor stub

}

RollingValueStore::~RollingValueStore() {
	// TODO Ahauto-generated destructor stub
}

void RollingValueStore::deleteEntry(RollingValueStoreEntry* ptr) {
	ptr->hints->clear();
	ptr->samples->clear();
	delete ptr;
	ptr=NULL;

}


bool RollingValueStore::retrieve(char* key,int low,int high,vector<float>** ptr)
{
	printf("Reached here\n");
	if(exists(key)==true)
	{

		vector<float>* samples= store[key]->samples;
		int numSamples = samples->size();

		if(numSamples<low || (high<low)){

			return false;
		}
		else {

			int maxIndex = high>(numSamples-1)?numSamples-1:high;

			int i=0;

			*ptr= new vector<float>(maxIndex-low+1);



			for(i=low;i<=high&&i<numSamples;i++)
			{
				(*ptr)->at(i-low)=samples->at(i);
				//printf("%f",ptr->at(i-low));
			}

			fflush(stdout);

			return true;

		}


	}
	else
	{
		printf("Key does not exist\n");
		return false;
	}

}


void RollingValueStore::declareKey(char* key,int numSamples)
{
	RollingValueStoreEntry* newEntry=NULL;

	if(exists(key)==true)
	{
		//flush the earlier entry and create a new one
		RollingValueStoreEntry* ptr = store[key];
		deleteEntry(ptr);
		newEntry = createEntry(numSamples);
	}
	else
	{
		//create a new entry
		newEntry = createEntry(numSamples);

	}
	cout << key << endl;
	store.insert(pair<string,RollingValueStoreEntry*>(key,newEntry));
	//store[key]=newEntry;
}



bool RollingValueStore::exists(char* key)
{

	if(store.find(key)==store.end())
		{
			return false;
		}
				else
		{
				return true;
		}
}

RollingValueStoreEntry* RollingValueStore::createEntry(int numSamples)
{
	RollingValueStoreEntry* ptr;
	ptr = new RollingValueStoreEntry;
	ptr->isFull=false;
	ptr->n=numSamples;
	ptr->newPosToInsert=0;
	ptr->samples = new vector<float>(numSamples);
	ptr->hints = new map<int,float>();
	return ptr;
}


bool RollingValueStore::addRollingValues(char* key,vector<float>* newSamplesPtr)
{
	if(exists(key)==true)
	{
		RollingValueStoreEntry* entry = store[key];
		int numElementsForKey = entry->n;
		int numNewElements = newSamplesPtr->size();
		int numElementsToInsert = numElementsForKey < numNewElements ? numElementsForKey:numNewElements;
		int startPos = numNewElements-numElementsForKey >= 0 ? numNewElements-numElementsForKey :0;



		int i;
		for(i=0;i<numElementsToInsert;i++)
		{

			entry->samples->at((entry->newPosToInsert+i)%numElementsForKey)=newSamplesPtr->at(startPos+i);
		}


		if((entry->newPosToInsert+numElementsToInsert)>=numElementsForKey)
		{
			entry->isFull=true;
		}
		entry->newPosToInsert = (entry->newPosToInsert+numElementsToInsert)%numElementsForKey;

		updateHints(entry);
		return true;
	}
	else
	{
		return false;
	}


}

bool RollingValueStore::calculateMovingAvgWithoutHint(int numSamples, RollingValueStoreEntry* current,float* result) {



	int actualNoOfSamples = numSamples > current->n ? current->n : numSamples;
	actualNoOfSamples =
			(current->isFull == false) ?
					current->newPosToInsert : actualNoOfSamples;
	float sum = 0;
	int i = 0;
	int ind = current->newPosToInsert - 1;
	for (i = 0; i < actualNoOfSamples; i++) {
		if (ind < 0) {
			ind = ind + current->n;
		}
		sum += current->samples->at(ind);
		ind = ind - 1;
	}
	if (actualNoOfSamples == 0) {
		//calculating average in this case would cause a divide by zero exception hence return error
		return false;
	} else {
		*result = (sum / actualNoOfSamples);
		return true;
	}

}

bool RollingValueStore::getRollingAvg(char* key,int numSamples,float* result)
{

		if(exists(key)==true)
		{
			RollingValueStoreEntry* current = store[key];

			if(current->hints->find(numSamples)==current->hints->end())
			{

				//not found in hints so calculate
				return calculateMovingAvgWithoutHint(numSamples, current,result);

			}
			else
			{
				//found in hints so return than value
				*result= (*current->hints)[numSamples];
				return true;
			}

		}
		else
		{
			return false;
		}



}


bool RollingValueStore::addHint(char* key,int numSamples)
{
		if(exists(key)==true)
		{
			map<int,float>* hints= store[key]->hints;
			float result;
			bool temp =getRollingAvg(key,numSamples,&result);
			if(temp==true)
			{
				(*hints)[numSamples]=result;
				return true;

			}
			else
			{
				return false;
			}

		}
		else
		{
			return false;
		}

}

void RollingValueStore::updateHints(RollingValueStoreEntry* ptr)
{

	map<int,float>* hints= ptr->hints;
	for(map<int,float>::iterator iter = hints->begin(); iter != hints->end(); ++iter)
	{
		float result;
		bool temp = calculateMovingAvgWithoutHint(iter->first,ptr,&result);
		if(temp==true)
		{
			(*hints)[iter->first]=result;
		}
		else
		{
			//something wrong in recalculating hints. removing the hint key,so next time it is re-computed
			hints->erase(iter->first);
		}

	}

}


