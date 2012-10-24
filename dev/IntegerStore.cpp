#include "IntegerStore.h"

IntegerStore::IntegerStore(Store* store) {
	this->store = store;

}

IntegerStore::~IntegerStore() {
	// TODO Auto-generated destructor stub
}


bool IntegerStore::get(char* query,int** result){

	bool exists = this->store->get(query,(void**)result,IntegerValue);
	if(exists){
		return true;
	}
	else {
		return false;
	}
}

bool IntegerStore::put(char* query,int val){

	int* earlier = NULL;
	bool earlierExists = this->store->get(query,(void**)&earlier,IntegerValue);
	int* valPtr = new int(val);
	bool success = this->store->put(query,valPtr,IntegerValue);

	if(success) {

		if(earlierExists) {
			delete earlier;
		}
		return true;
	}
	else {
		delete valPtr;
		return false;
	}

}

bool IntegerStore::increment(char* query,int val,int** result)
{
	bool exists = this->store->get(query,(void**)result,IntegerValue);
	if(exists){
		**result = **result + val;
		return true;
	}
	else {
		return false;
	}
}

bool IntegerStore::decrement(char* query,int val,int** result){

	return increment(query,-val,result);
}


