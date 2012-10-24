/*
 * StringParserandInvoker.h
 *
 *  Created on: Oct 3, 2012
 *      Author: supriya
 */

#include<string>

#include "RollingValueStore.h"
#include "IntegerStore.h"


#ifndef STRINGPARSERANDINVOKER_H_
#define STRINGPARSERANDINVOKER_H_

using namespace std;

enum InputType
{
	incrementvalue=1,
	decrementvalue,
	getvalue,
	putvalue,
	declsamples,
	declmovavg,
	addsamples,
	movavg,
	retrieve,
	variance
};

class StringParserandInvoker {
private:
	IntegerStore is;
	RollingValueStore rvs;
	char* str;
	void mapValues();
	string put(vector<string> tokens);
	string get(vector<string> tokens);
	string increment(vector<string> tokens);
	string decrement(vector<string> tokens);
	string declSamples(vector<string> tokens);
	string declMovAvg(vector<string> tokens);
	string addSamples(vector<string> tokens);
	vector<float> splitNumbers(string token);
	string movingAvg(vector<string> tokens);
	map<string,InputType> enumMapping;
	string retrieveN(vector<string> tokens);
	string getvariance(vector<string> tokens);
	string removeNewLineCharactersAtEndOfLine(string* message);
public:
	StringParserandInvoker();
	StringParserandInvoker(IntegerStore, RollingValueStore);
	string operate(string);
	virtual ~StringParserandInvoker();
};

#endif /* STRINGPARSERANDINVOKER_H_ */
