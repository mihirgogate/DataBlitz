
#include <string>
#include<iostream>

#include "RollingValueStore.h"
#include "IntegerStore.h"
#include "StringParserandInvoker.h"
#include "server.h"

using namespace std;


int main(int argc, char *argv[])
{
	if (argc != 2)
	{
	  fprintf (stderr, "Error: Usage : %s [port]\n", argv[0]);
	  return 1;
	}

	IntegerStore is;
	RollingValueStore rvs;
	StringParserandInvoker spi(is,rvs);

	server_main(argv[1],&spi);

/*  Sample tests below
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







