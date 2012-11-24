#include<string>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<sstream>

#include "JSONParserAndInvoker.h"
using namespace std;


JSONParserAndInvoker::JSONParserAndInvoker(IntegerStore* is, RollingValueStore* rs) {
	this->is=is;
	this->rvs=rs;
}

//request :{"command":"GET","key":"test"}
//response:{"status":"success","result":<num>}
string JSONParserAndInvoker::get(Json::Value root)
{
	Json::Value error;
	if(!root.isMember("key") || !root["key"].isString()) {
		error = createErrorJsonObject("Invalid format. Expected 'key'.");
		return error.toStyledString();
	}
	else {
		Json::Value rootAns;
		int* result;
		bool isSuccess = this->is->get((char*)(root["key"].asCString()),&result);
		if(isSuccess) {
			rootAns["status"]="success";
			rootAns["result"]=*result;
			return rootAns.toStyledString();
		}
		else {
			error = createErrorJsonObject("Key not found");
			return error.toStyledString();
		}

	}
}

//request :{"command":"PUT","key":"test","value":<num>}
//response:{"status":"success"}
string JSONParserAndInvoker::put(Json::Value root)
{
	Json::Value error;
	if(!root.isMember("key") || !root["key"].isString() || !root.isMember("value") || !root["value"].isInt()) {
		error = createErrorJsonObject("Invalid format. Expected 'key' and integer 'value' .");
		return error.toStyledString();
	}
	else {
		Json::Value rootAns;
		bool isSuccess = this->is->put((char*)(root["key"].asCString()),root["value"].asInt());
		if(isSuccess) {
			rootAns["status"]="success";
			return rootAns.toStyledString();
		}
		else {
			error = createErrorJsonObject("Key not found");
			return error.toStyledString();
		}

	}
}

//request :{"command":"DECLSAMPLES","key":"test","numSamples":<value>}
//response:{"status":"success"}
string JSONParserAndInvoker::declsamples(Json::Value root)
{
	Json::Value error;

	if(!root.isMember("key") || !root["key"].isString() || !root.isMember("numSamples") || !root["numSamples"].isInt()){
		error = createErrorJsonObject("Invalid format. Expected 'key' and integer 'numSamples'");
		return error.toStyledString();
	}
	else {
		Json::Value rootAns;
		bool isSuccess = rvs->declareKey((char*)(root["key"].asCString()),root["numSamples"].asInt());
		if(isSuccess) {
			rootAns["status"]="success";
			return rootAns.toStyledString();
		}
		else {
			error = createErrorJsonObject("Key not found");
			return error.toStyledString();
		}

	}
}

//request :{"command":"ADDSAMPLES","key":"test","values":<json array of floats>}
//response:{"status":"success"}
string JSONParserAndInvoker::addsamples(Json::Value root)
{
	Json::Value error;
	int i=0;

	if(!root.isMember("key") || !root["key"].isString() || !root.isMember("values") || !root["values"].isArray()){
		error = createErrorJsonObject("Invalid format. Expected 'key' and an array 'values'.");
		return error.toStyledString();
	}
	else {
		Json::Value rootAns;
		Json::Value jsonArray =root["values"];
		int size = jsonArray.size();
		vector<float> numbers(size);
		for(i=0;i<size;i++) {
			numbers.push_back(jsonArray[i].asDouble());
		}
		bool isSuccess = rvs->addRollingValues((char*)(root["key"].asCString()),&numbers);
		if(isSuccess) {
			rootAns["status"]="success";
			return rootAns.toStyledString();
		}
		else {
			error = createErrorJsonObject("Key not found");
			return error.toStyledString();
		}

	}
}

//request :{"command":"HISTOGRAM","key":"test"}
//response:{"status":"success","entries":<numEntries>,"values":<json array of floats>,"frequencies":<json array of ints>}
string JSONParserAndInvoker::histogram(Json::Value root)
{
	Json::Value error;
	int i=0;

	if(!root.isMember("key") || !root["key"].isString()){
		error = createErrorJsonObject("Invalid format. Expected 'key'.");
		return error.toStyledString();
	}
	else {
		Json::Value rootAns;
		float* uniqueVals;
		int* frequencies;
		int* count;
		bool isSuccess = rvs->getHistogram((char*)(root["key"].asCString()),&uniqueVals,&frequencies,&count);
		if(isSuccess) {
			rootAns["status"]="success";
			rootAns["entries"]=*count;
			for(i=0;i<*count;i++) {
				rootAns["values"][i]=uniqueVals[i];
				rootAns["frequencies"][i]=frequencies[i];
			}


			return rootAns.toStyledString();
		}
		else {
			error = createErrorJsonObject("Key not found");
			return error.toStyledString();
		}

	}
}

string JSONParserAndInvoker::operate(string jsonString)
{
	Json::Value error;
	Json::Value root;
	Json::Reader reader;

	bool isSuccess = reader.parse(jsonString,root);
	if(!isSuccess) {
		error = createErrorJsonObject("Not valid json JSON input");
		return error.toStyledString();
	}
	else {


		if(!root.isMember("command") || !root["command"].isString()) {
			error = createErrorJsonObject("No valid command present in input");
			return error.toStyledString();
		}
		else {
				string command = root["command"].asString();

				if(command.compare("GET")==0) {
					return get(root);
				}
				if(command.compare("PUT")==0) {
					return put(root);
				}

				if(command.compare("DECLSAMPLES")==0) {
					return declsamples(root);
				}
				if(command.compare("ADDSAMPLES")==0) {
					return addsamples(root);
				}
				if(command.compare("HISTOGRAM")==0) {
					return histogram(root);
				}

				error = createErrorJsonObject("Unknown command present in input");
				return error.toStyledString();

			}

		}
}


Json::Value JSONParserAndInvoker::createErrorJsonObject(string errorMessage)
{
	Json::Value root;

	root["status"]="error";
	root["errorMessage"]=errorMessage;

	return root;
}
