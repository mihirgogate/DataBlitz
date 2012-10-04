/*
 * RollingValueStore.h
 *
 *  Created on: Sep 20, 2012
 *      Author: mihirgogate
 */

#include<stdio.h>
#include<string>
#include<map>
#include<vector>

using namespace std;
#ifndef ROLLINGVALUESTORE_H_
#define ROLLINGVALUESTORE_H_

struct RollingValueStoreEntry
{
	int n;
	vector<float>* samples;
	map<int,float>* hints;
	bool isFull;
	int newPosToInsert;
};

class RollingValueStore {

private:

	map<string,RollingValueStoreEntry*> store;
	RollingValueStoreEntry* createEntry(int numSamples);
	void deleteEntry(RollingValueStoreEntry* ptr);
	void updateHints(RollingValueStoreEntry* ptr);
	bool calculateMovingAvgWithoutHint(int numSamples,RollingValueStoreEntry* current,float* result);

public:
	RollingValueStore();
	virtual ~RollingValueStore();
	void declareKey(char* key,int numSamples);
	bool addRollingValues(char* key,vector<float>* ptr);
	bool exists(char* key);
	bool getRollingAvg(char* key,int numSamples,float* result);
	bool addHint(char* key,int numSamples);
};




#endif /* ROLLINGVALUESTORE_H_ */
