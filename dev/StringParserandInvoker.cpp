/*
 * StringParserandInvoker.cpp
 *
 *  Created on: Oct 3, 2012
 *      Author: supriya
 */
#include<string>
#include<iostream>
#include<stdlib.h>
#include<sstream>

#include "StringParserandInvoker.h"
#include "RollingValueStore.h"
#include "IntegerStore.h"


StringParserandInvoker::StringParserandInvoker() {

	str=NULL;

}

StringParserandInvoker::StringParserandInvoker(IntegerStore is, RollingValueStore rs) {
	this->is=is;
	this->rvs=rs;
	str=NULL;
}

string StringParserandInvoker::operate(string message)
{
	vector<string> tokens;
	string buf="";
	unsigned int i=0;
	bool additionalSpace=true;
	while(i<message.length())
	{
		if(message[i]!=' ')
		{
			buf+=message[i];
			additionalSpace=false;
		}
		else
		{
			if(!additionalSpace)
			{
			tokens.push_back(buf);
			buf="";
			additionalSpace=true;
			}
		}
		i++;
	}
	if(!buf.empty())
	{
		tokens.push_back(buf);
	}
	/*

	for(i=0;i<tokens.size();i++)
	{
		cout<<tokens[i]<<endl;
	}
	cout << tokens[0] << endl;
	*/
	mapValues();
	string returnVal;
	//cout << enumMapping[tokens[0]]<<endl;
	switch(enumMapping[tokens[0]])
	{
	case putvalue:
		//cout << "PUT" << endl;
		returnVal=put(tokens);
		break;
	case getvalue:
		returnVal=get(tokens);
		break;
	case incrementvalue:
		//cout << "INCR" << endl;
		returnVal=increment(tokens);
		break;
	case decrementvalue:
		//cout << "DECR" << endl;
		returnVal=decrement(tokens);
		break;
	case declsamples:
		returnVal=declSamples(tokens);
		break;
	case declmovavg:
		returnVal=declMovAvg(tokens);
		break;
	case addsamples:
		returnVal=addSamples(tokens);
		break;
	case movavg:
		returnVal=movingAvg(tokens);
		break;
	case retrieve:
		returnVal = retrieveN(tokens);
		break;
	default:
		returnVal="Invalid input";
		break;
	}
	return returnVal;

}

string StringParserandInvoker:: retrieveN(vector<string> tokens)
{
	if(tokens.size()==4)
	  {
				str=(char*)tokens[1].c_str();
				int lowIndex=atoi(tokens[2].c_str());
				int highIndex=atoi(tokens[3].c_str());
				vector<float>* result;
				bool isSuccess= rvs.retrieve(str,lowIndex,highIndex,&result);
				int i=0;
				if(isSuccess)
				{
					std::stringstream out;
					printf("%d\n",result->size());
					for(i=0;i<result->size();i++)
					{
						//printf("%f",result->at(i));
						out<<result->at(i);
						if(i!=(result->size()-1))
						{
							out<<",";
						}

					}
					fflush(stdout);
					return out.str();
				}
				else {
					return "Invalid parameters supplied";
				}

	  }
	  else
	  {
		return "Invalid number of parameters. /n Usage RETRIEVE <key> <lowIndex> <highIndex>";
	  }

}


string StringParserandInvoker:: put(vector<string> tokens)
{
	if(tokens.size()==3)
			{
				str=(char*)tokens[1].c_str();
				int number=atoi(tokens[2].c_str());
				is.put(str,number);
				return "Value added successfully!!";
			}
			else
			{
				return "Invalid number of parameters. /n Usage PUT <data> <value>";
			}

}

string StringParserandInvoker:: get(vector<string> tokens)
{
	if(tokens.size()==2)
			{
				str=(char*)tokens[1].c_str();
				int result;
				bool isValid=is.get(str,&result);
				if(isValid)
				{
					std::stringstream out;
					out<<result;
					return out.str();
				}
				else
				{
					return "Value not found";
				}

			}
			else
			{
				return "Invalid number of parameters. /n Usage GET <data>";
			}

}
string StringParserandInvoker:: increment(vector<string> tokens)
{
	if(tokens.size()==3)
			{
				str=(char*)tokens[1].c_str();
				int number=atoi(tokens[2].c_str());
				int result;
				bool isValid=is.increment(str,number,&result);
				if(isValid)
				{
					std::stringstream out;
					out<<result;
					return out.str();
				}
				else
				{
					return "Value not found";
				}

			}
			else
			{
			return "Invalid number of parameters. /n Usage INCREMENT <data> <value>";

		}

}
string StringParserandInvoker:: decrement(vector<string> tokens)
{
	if(tokens.size()==3)
			{
				str=(char*)tokens[1].c_str();
				int number=atoi(tokens[2].c_str());
				int result;
				bool isValid=is.decrement(str,number,&result);
				if(isValid)
				{
					std::stringstream out;
					out<<result;
					return out.str();
				}
				else
				{
					return "Value not found";
				}

			}
			else
			{
			return "Invalid number of parameters. /n Usage DECREMENT <data> <value>";
			}

}

string StringParserandInvoker:: declSamples(vector<string> tokens)
{
	if(tokens.size()==3)
	{

			str=(char*)tokens[1].c_str();
			int number=atoi(tokens[2].c_str());
			rvs.declareKey(str,number);
			return "Successful";
	}
	return "Invalid number of parameters. /n Usage DECLSAMPLES <data> <value>";
}

string StringParserandInvoker:: declMovAvg(vector<string> tokens)
{
	if(tokens.size()==3)
	{
			str=(char*)tokens[1].c_str();
			int number=atoi(tokens[2].c_str());
			bool isTrue=rvs.addHint(str,number);
			if(isTrue)
			{
			return "Successful";
			}
			else
			{
				return "Value not found";
			}
	}
	return "Invalid number of parameters. /n Usage DECLSAMPLES <data> <value>";
}

string StringParserandInvoker:: addSamples(vector<string> tokens)
{
	vector<float> numbers;
	if(tokens.size()==3)
		{
				str=(char*)tokens[1].c_str();
				numbers=splitNumbers(tokens[2]);
				bool isTrue=rvs.addRollingValues(str,&numbers);
				if(isTrue)
				{
				return "Successful";
				}
				else
				{
					return "Value not found";
				}
		}
		return "Invalid number of parameters. /n Usage DECLSAMPLES <data> <value>";

}

vector<float> StringParserandInvoker:: splitNumbers(string token)
{
	vector<float> numbers;
		string buf="";
		unsigned int i=0;
		bool additionalComma=true;
		while(i<token.length())
		{
			if(token[i]!=',')
			{
				buf+=token[i];
				additionalComma=false;
			}
			else
			{
				if(!additionalComma)
				{
				numbers.push_back(atoi(buf.c_str()));
				buf="";
				additionalComma=true;
				}
			}
			i++;
		}
		if(!buf.empty())
		{
			numbers.push_back(atoi(buf.c_str()));
		}
return numbers;
}

string StringParserandInvoker:: movingAvg(vector<string> tokens)
{
	if(tokens.size()==3)
	{
		str=(char*)tokens[1].c_str();
		int number=atoi(tokens[2].c_str());
		float result;
		bool isValid=rvs.getRollingAvg(str,number,&result);
		if(isValid)
		{
			std::stringstream out;
			out<<result;
			return out.str();
		}
		else
		{
			return "Value not found";
		}
	}
	else
	{
		return "Invalid number of parameters. /n Usage MOVAVG <data> <value>";
	}
}

void StringParserandInvoker::mapValues()
{
enumMapping["PUT"]=putvalue;
enumMapping["GET"]=getvalue;
enumMapping["INCREMENT"]=incrementvalue;
enumMapping["DECREMENT"]=decrementvalue;
enumMapping["DECLSAMPLES"]=declsamples;
enumMapping["DECLMOVAVG"]=declmovavg;
enumMapping["ADDSAMPLES"]=addsamples;
enumMapping["MOVAVG"]=movavg;
enumMapping["RETRIEVE"]=retrieve;

}

StringParserandInvoker::~StringParserandInvoker() {
	// TODO Auto-generated destructor stub
}

