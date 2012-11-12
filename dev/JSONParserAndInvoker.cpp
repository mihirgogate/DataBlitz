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
