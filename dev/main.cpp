
#include <string>
#include<iostream>

#include "RollingValueStore.h"
#include "IntegerStore.h"
#include "StringParserandInvoker.h"
#include "server.h"
#include "Store.h"

using namespace std;


int main(int argc, char *argv[])
{

	if (argc != 2)
	{
	  fprintf (stderr, "Error: Usage : %s [port]\n", argv[0]);
	  return 1;
	}

	Store* s = new Store();
	IntegerStore* is = new IntegerStore(s);
	RollingValueStore* rvs = new RollingValueStore();
	StringParserandInvoker spi(is,rvs);

	server_main(argv[1l],&spi);

/*
 * Sample tests for Store below
	string a = "test";
	int* ptr = new int(2);
	bool is = s.exists(&a[0],IntegerValue);
	is = s.put(&a[0],ptr,IntegerValue);
	is = s.exists(&a[0],IntegerValue);
	int* ans;
	is  = s.get(&a[0],(void**)&ans,IntegerValue);
	ptr = new int(5);
	is = s.put(&a[0],ptr,IntegerValue);
	is  = s.get(&a[0],(void**)&ans,IntegerValue);
	printf("%d,%d",is,*ans);

 *
 *  Sample tests below
	string message="      PUT    test 15      ";
	string returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="GET test";
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;

	message="INCREMENT   test 10";
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="DECREMENT test 5";
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="DECLSAMPLES test 5";
	cout<< message << endl;
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="ADDSAMPLES test 1,1";
	cout<< message << endl;
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="DECLMOVAVG test 3";
	cout<< message << endl;
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="MOVAVG test 3";
	cout<< message << endl;
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="ADDSAMPLES test 4";
	cout<< message << endl;
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="MOVAVG test 3";
	cout<< message << endl;
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="ADDSAMPLES test 4,4,4";
	cout<< message << endl;
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="MOVAVG test 3";
	cout<< message << endl;
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;
	message="MOVAVG test 4";
	cout<< message << endl;
	returnMessage = spi.operate(message);
	cout << returnMessage <<endl;

*/
	return 0;
}







