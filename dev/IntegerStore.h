
#include<map>
#include<string>

#ifndef DATASTORE_H_
#define DATASTORE_H_

using namespace std;

class IntegerStore {

private :

	map<string,int> stats;

public:
	IntegerStore();
	bool get(const char *c,int* result);
	void put(const char *c,int value);
	bool increment(const char *c,int value,int* result);
	bool decrement(const char *c,int value,int* result);
	bool exists(const char *c);
	virtual ~IntegerStore();
};

#endif /* DATASTORE_H_ */
