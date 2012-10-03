#include "IntegerStore.h"

IntegerStore::IntegerStore() {
	// TODO Auto-generated constructor stub

}

IntegerStore::~IntegerStore() {
	// TODO Auto-generated destructor stub
}


bool IntegerStore::get(const char* query,int* result){

	if(exists(query)==false)
		{
			return false;
		}
		else
		{
			*result=stats[query];
			return true;
		}
}

void IntegerStore::put(const char* query,int val){

	stats[query]=val;


}

bool IntegerStore::increment(const char* query,int val,int *result)
{

	if(exists(query)==false)
	{
		return false;
	}
	else
	{
		stats[query] = stats[query] + val;
		*result=stats[query];
		return true;
	}
}

bool IntegerStore::decrement(const char* query,int val,int *result){

	return increment(query,-val,result);
}



bool IntegerStore::exists(const char* query){

	      if(stats.find(query)==stats.end())
			{
				return false;
			}
			else
			{
				return true;
			}

}

