/*
 * RollingValueStore.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: mihirgogate
 */

#include "RollingValueStore.h"
#include<iostream>
#include<string>
#include<math.h>

RollingValueStore::RollingValueStore(Store* store) {
	this->store = store;

}

RollingValueStore::~RollingValueStore() {
	// TODO Ahauto-generated destructor stub
}

void RollingValueStore::deleteEntry(RollingValueStoreEntry* ptr) {
	ptr->hints->clear();
	ptr->samples->clear();
	delete ptr;
	ptr=NULL;
	//TODO:Free the histogram nodes

}

bool RollingValueStore :: getVariance(char* key,float* result)
{
	RollingValueStoreEntry* current;
    bool exists = store->get(key,(void**)&current,RollingValue);
    if(exists)
    {
        float avg;
        int ind = current->newPosToInsert - 1;

        if (ind < 0) {
            ind = ind + current->n;
        }

        int actualNoOfSamples = getActualNumberOfSamples(current);
        bool isTrue=getRollingAvg(key,actualNoOfSamples,&avg);
        if(isTrue)
        {
            	float sum = 0;
                int i = 0;
                float num=0;
                for (i = 0; i < actualNoOfSamples; i++) {
                    if (ind < 0) {
                        ind = ind + current->n;
                    }
                    num=current->samples->at(ind)-avg;
                    sum +=num*num ;
                    ind = ind - 1;
                }

                *result=sum/(float)(actualNoOfSamples);


                return true;
        }
        else
        {
            return false;  //Returning false as the actual number of samples is 0
        }
    }
    else
    {
        return false;
    }
}

bool RollingValueStore :: getStdDev(char* key,float* result)
{
	bool isVarianceSuccessfullyRetrieved  = getVariance(key,result);
	if(isVarianceSuccessfullyRetrieved) {
		*result = sqrt(*result);
		return true;
	}
	else {
		return false;
	}
}

int RollingValueStore::getActualNumberOfSamples(RollingValueStoreEntry* rollingValueStoreEntry) {
	return (rollingValueStoreEntry->isFull == false) ? rollingValueStoreEntry->newPosToInsert : rollingValueStoreEntry->samples->size();
}

bool RollingValueStore::retrieve(char* key,int low,int high,vector<float>** ptr)
{
	RollingValueStoreEntry* current;
	bool exists = store->get(key,(void**)&current,RollingValue);
	if(exists)
	{

		vector<float>* samples= current->samples;
		int actualNoOfSamples = getActualNumberOfSamples(current);

		if(actualNoOfSamples<=low || (high<low) || (low<0) || (high<0)){

			return false;
		}
		else {

			int maxIndex = high>(actualNoOfSamples-1)?actualNoOfSamples-1:high;


			int i=0;

			*ptr= new vector<float>(maxIndex-low+1);



			for(i=low;i<=high&&i<actualNoOfSamples;i++)
			{
				(*ptr)->at(i-low)=samples->at(i);
			}


			return true;

		}


	}
	else
	{
		return false;
	}

}


bool RollingValueStore::declareKey(char* key,int numSamples)
{
	RollingValueStoreEntry* current;
	bool exists = store->get(key,(void**)&current,RollingValue);

	RollingValueStoreEntry* newEntry=newEntry = createEntry(numSamples);
	bool isSuccess = store->put(key,newEntry,RollingValue);
	if(isSuccess)
	{
		//free the memory of the earlier entry if it exists
		if(exists) {
			deleteEntry(current);
		}

		return true;
	}
	else
	{
		//could not add the rolling value to the store so free the memory
		deleteEntry(newEntry);
		return false;

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
	ptr->lifetimeCount =0;
	ptr->lifetimeSum =0;
	ptr->histogramRoot=NULL;
	ptr->numUniqueValuesInHistogram=0;
	return ptr;
}

bool RollingValueStore::getLifetimeAverage(char* key,float* result){
	RollingValueStoreEntry* entry=NULL;
	bool exists = store->get(key,(void**)&entry,RollingValue);
	if(exists){
		if(entry->lifetimeCount == 0) {
			return false;
		}
		else {
			*result = (entry->lifetimeSum)/(entry->lifetimeCount);
			return true;
		}
	}
	else {
		return false;
	}
}

void RollingValueStore::updateHistogram(char* key,vector<float>* newSamplesPtr){

	RollingValueStoreEntry* entry=NULL;
	bool exists = store->get(key,(void**)&entry,RollingValue);
	if(exists) {

		int i=0;
		for(i=0;i<newSamplesPtr->size();i++)
		{
			insertValueInHistogram(key,newSamplesPtr->at(i));
		}
	}

}

bool RollingValueStore::getHistogram(char* key,float** uniqueVals,int** frequencies,int** count) {


	RollingValueStoreEntry* entry=NULL;
	bool exists = store->get(key,(void**)&entry,RollingValue);
	if(exists) {

		*count = new int;
		**count  = entry->numUniqueValuesInHistogram;

		*uniqueVals = new float[**count];
		*frequencies = new int[**count];


		HistogramNode* current = entry->histogramRoot;
		int index = 0;
		while(current!=NULL) {

				(*uniqueVals)[index] = current->uniqueNum;
				(*frequencies)[index]= current->count;
				current = current->next;
				index++;

		}
		return true;
	}
	else {
		return false;
	}


}

void RollingValueStore::insertValueInHistogram(char* key,int num)
{
		RollingValueStoreEntry* entry=NULL;
		bool exists = store->get(key,(void**)&entry,RollingValue);
		if(exists) {

				HistogramNode* prev = NULL;
				HistogramNode* current = entry->histogramRoot;


			while(current!=NULL) {

			if(current->uniqueNum>num) {
					break;
				}
				prev = current;
				current = current->next;
			}

			if(prev!=NULL) {

				if(prev->uniqueNum==num) {
					prev->count++;
				}
				else {
					//create a new node
					HistogramNode* newNode = new HistogramNode();
					newNode->uniqueNum = num;
					newNode->count = 1;
					newNode->next=current;
					prev->next=newNode;
					entry->numUniqueValuesInHistogram = entry->numUniqueValuesInHistogram + 1;
				}

			}
			else {
					HistogramNode* newNode = new HistogramNode();
					newNode->uniqueNum = num;
					newNode->count = 1;
					newNode->next=current;
					entry->histogramRoot = newNode;
					entry->numUniqueValuesInHistogram = entry->numUniqueValuesInHistogram + 1;
			}

		}


}

void RollingValueStore::updateLifeTimeValues(char* key,vector<float>* newSamplesPtr)
{
	RollingValueStoreEntry* entry=NULL;
	bool exists = store->get(key,(void**)&entry,RollingValue);
	if(exists) {
		float sum = 0;
		int count = 0;
		int i;
		for(i=0;i<newSamplesPtr->size();i++)
		{
			sum+=newSamplesPtr->at(i);
			count+=1;
		}

		//now update the lifetime sum and count
		entry->lifetimeCount += count;
		entry->lifetimeSum += sum;
	}
}

bool RollingValueStore::addRollingValues(char* key,vector<float>* newSamplesPtr)
{
	RollingValueStoreEntry* entry=NULL;

	updateLifeTimeValues(key,newSamplesPtr);
	updateHistogram(key,newSamplesPtr);

	bool exists = store->get(key,(void**)&entry,RollingValue);


	if(exists)
	{
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


	int actualNoOfSamples = getActualNumberOfSamples(current);

	int numSamplesToTakeAverageOf = numSamples < actualNoOfSamples ? numSamples : actualNoOfSamples;

	float sum = 0;
	int i = 0;
	int ind = current->newPosToInsert - 1;
	for (i = 0; i < numSamplesToTakeAverageOf; i++) {
		if (ind < 0) {
			ind = ind + current->n;
		}
		sum += current->samples->at(ind);
		ind = ind - 1;
	}
	if (numSamplesToTakeAverageOf == 0) {
		//calculating average in this case would cause a divide by zero exception hence return error
		return false;
	} else {
		*result = (sum / numSamplesToTakeAverageOf);
		return true;
	}

}

bool RollingValueStore::getRollingAvg(char* key,int numSamples,float* result)
{

		RollingValueStoreEntry* current=NULL;
		bool exists = store->get(key,(void**)&current,RollingValue);

		if(exists)
		{

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
		RollingValueStoreEntry* current=NULL;
		bool exists = store->get(key,(void**)&current,RollingValue);

		if(exists)
		{
			map<int,float>* hints= current->hints;
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
			//TODO : something wrong in recalculating hints. removing the hint key,so next time it is re-computed
			hints->erase(iter->first);
		}

	}

}


