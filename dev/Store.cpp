/*
 * Store.cpp
 *
 *  Created on: Oct 23, 2012
 *      Author: mihirgogate
 */

#include "Store.h"

Store::Store() {
	// TODO Auto-generated constructor stub

}

Store::~Store() {
	// TODO Auto-generated destructor stub
}

bool Store::exists(char* key,StoreDataType dataType){

	StoreEntry* storeEntry = store[key];
    if(storeEntry==NULL)
    {
    	return false;
    }
    else {
    	if(storeEntry->dataType!=dataType) {
    		return false;
    	}
    	else {
    		return true;
    	}
    }
}


bool Store::get(char* key,void** ptr,StoreDataType dataType)
{
	if(!exists(key,dataType)) {
		return false;
	}
	else {
		*ptr = store[key]->ptr;
 		return true;
	}
}

bool Store::put(char* key,void* ptr,StoreDataType dataType)
{
	//if a value of a different type already exists for that key then return false
	if(store[key]!=NULL && store[key]->dataType!=dataType) {
		return false;
	}
	else {
		StoreEntry* storeEntry = new StoreEntry();
		storeEntry->dataType = dataType;
		storeEntry->ptr = ptr;
		store[key] = storeEntry;
 		return true;
	}
}

