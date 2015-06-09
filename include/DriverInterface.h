/*
 * Plugin_Interface.h
 *
 *  Created on: 21.01.2015
 *      Author: dnoack
 */

#ifndef DRIVERINTERFACE_H_
#define DRIVERINTERFACE_H_


#include <map>
#include <list>
#include <cstring>

#include "document.h"

#include "Error.hpp"

using namespace std;
using namespace rapidjson;

struct _param
{
	const char* _name;
	rapidjson::Type _type;
};



template <class TDriver, class TPointer>
class DriverInterface{

	public:
		DriverInterface(TDriver derivedClass)
		{
			driver = derivedClass;
		};

		~DriverInterface()
		{
			funcMap.clear();
		};


		bool executeFunction(Value &method, Value &params, Value &result)
		{
			try{
				funcP = funcMap[(char*)method.GetString()];
				if(funcP == NULL)
					throw Error("Function not found.",  __FILE__, __LINE__);
				else
					return (driver->*funcP)(params, result);
			}
			catch(const Error &e)
			{
				throw;
			}
		}


		list<string*>* getAllFunctionNames()
		{
			list<string*>* funcList = new list<string*>();


			for(typename map<const char*,TPointer>::iterator it=funcMap.begin(); it != funcMap.end(); it++)
			{
				funcList->push_back(new string(it->first));
			}
			return funcList;
		}


	protected:

		struct cmp_keys
		{
			bool operator()(char const* input, char const* key)
			{
				return strcmp(input, key) < 0;
			}
		};


		//use char* for keys, because rapidjson gives us char* and we save a lot of allocating new strings
		map<const char*, TPointer, DriverInterface::cmp_keys> funcMap;
		TPointer funcP;
		TDriver driver;
};



#endif /* PLUGIN_ORIGIN_INCLUDE_PLUGIN_INTERFACE_H_ */
