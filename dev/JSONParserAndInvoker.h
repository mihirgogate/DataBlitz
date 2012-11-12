#include<string>
#include "IntegerStore.h"
#include "RollingValueStore.h"
#include "jsonCPP/include/json.h"

using namespace std;

class JSONParserAndInvoker
{
	private:
	IntegerStore* is;
	RollingValueStore* rvs;
	Json::Value createErrorJsonObject(string errorMessage);
	string get(Json::Value root);
	string put(Json::Value root);

	public:
	string operate(string jsonString);
	JSONParserAndInvoker(IntegerStore* is, RollingValueStore* rs) ;


};
