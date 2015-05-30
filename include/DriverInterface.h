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
#include "writer.h"

#include "Error.hpp"
#include "JsonRPC.hpp"

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



		Value* findObjectMember(Value &object, const char* memberName)
		{
			Type paramsType;
			Value* member;

			paramsType = object.GetType();
			if(paramsType == kObjectType)
			{
				if(object.HasMember(memberName))
				{
					member = &(object[memberName]);
					return member;
				}
				else
					throw Error("Needed member not found.",  __FILE__, __LINE__);
			}
			else
				throw Error("Params is not an Object.",  __FILE__, __LINE__);

		}


		//like above but with type check
		Value* findObjectMember(Value &object, const char* memberName, rapidjson::Type shouldBeType)
		{
			Type currentType;
			Value* member;

			currentType = object.GetType();
			if(currentType == kObjectType)
			{
				if(object.HasMember(memberName))
				{
					member = &(object[memberName]);
					if(member->GetType() == shouldBeType)
						return member;
					else
						throw Error("Member got incorrect type.",  __FILE__, __LINE__);
				}
				else
					throw Error("Needed member not found.",  memberName, __LINE__);
			}
			else
				throw Error("Params is not an Object.",  __FILE__, __LINE__);

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
