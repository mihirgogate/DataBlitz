
#include <string>
#include<iostream>

#include "DataStore.h"
#include "RollingValueStore.h"
#include "IntegerStore.h"
#include "StringParserandInvoker.h"

using namespace std;

int server_main (int argc, char *argv[],DataStore* ds);

int main(int argc, char *argv[])
{
	DataStore ds;
	server_main(argc,argv,&ds);
	IntegerStore is;
	RollingValueStore rvs;
	StringParserandInvoker spi(is,rvs);
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
	return 0;

	/*
	 *
	 * int res;
	 * bool isValid;
	 * isValid = increment("",1,&res)
	 * if(isValid)
	 * 	  //I can use res
	 * else
	 * 	  // res contains junk
	 */
}







