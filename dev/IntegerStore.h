
#include<map>
#include<string>
#include "Store.h"

#ifndef DATASTORE_H_
#define DATASTORE_H_

using namespace std;

class IntegerStore {

private :

	Store* store;

public:
	IntegerStore(Store* store);
	bool get(char *c,int** result);
	bool put(char *c,int value);
	bool increment(char *c,int value,int** result);
	bool decrement(char *c,int value,int** result);
	virtual ~IntegerStore();
};

#endif /* DATASTORE_H_ */
