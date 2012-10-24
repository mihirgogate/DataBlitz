/*
 * Store.h
 *
 *  Created on: Oct 23, 2012
 *      Author: mihirgogate
 */

#include<map>
#include<string>

#ifndef STORE_H_
#define STORE_H_

using namespace std;

enum StoreDataType {
		IntegerValue,
		RollingValue
	};

class StoreEntry
{

 public:
	StoreDataType dataType;
	void *ptr;

};


class Store {


private:

	map<string,StoreEntry*> store;

public:

	bool exists(char* key,StoreDataType dataType);
	bool get(char* key,void** ptr,StoreDataType dataType);
	bool put(char* key,void* ptr,StoreDataType dataType);
	Store();
	virtual ~Store();

};




#endif /* STORE_H_ */
