#include "stdio.h"
#include <JsonRPC.hpp>
#include "Error.hpp"


void JsonRPC::parse( Document* dom, string* msg)
{
	dom->Parse(msg->c_str());

	if(dom->HasParseError())
		throw Error(-32700, "Error while parsing json rpc.");
}


Value* JsonRPC::getParam(Document* dom, const char* name)
{
	Value* params = NULL;
	Value* result = NULL;

	params = &((*dom)["params"]);
	result = &((*params)[name]);

	return result;
}


Value* JsonRPC::tryTogetParam(Document* dom, const char* name)
{
	Value* result;
	Value* params;
	Value nullid;

	try
	{
		hasParams(dom);
		params = &((*dom)["params"]);
		if(!params->HasMember(name))
			throw Error(-32022, "Missing parameter.");
		else
			result = getParam(dom, name);

	}
	catch(Error &e)
	{
		throw;
	}

	return result;
}


Value* JsonRPC::getParams(Document* dom)
{
	return &((*dom)["params"]);
}


Value* JsonRPC::tryTogetParams(Document* dom)
{
	Value* params = NULL;

	try
	{
		hasParams(dom);
		params = getParams(dom);
	}
	catch(Error &e)
	{
		throw;
	}
	return params;
}


Value* JsonRPC::getResult(Document* dom)
{
	return &((*dom)["result"]);
}


Value* JsonRPC::tryTogetResult(Document* dom)
{
	Value* resultValue = NULL;

	try
	{
		hasResult(dom);
		resultValue = getResult(dom);
	}
	catch(Error &e)
	{
		throw;
	}

	return resultValue;
}


Value* JsonRPC::getMethod(Document* dom)
{
	return  &((*dom)["method"]);
}


Value* JsonRPC::tryTogetMethod(Document* dom)
{
	Value* methodValue = NULL;

	try
	{
		hasMethod(dom);
		methodValue = getMethod(dom);
	}
	catch(Error &e)
	{
		throw;
	}
	return methodValue;
}


Value* JsonRPC::getId(Document* dom)
{
	return &((*dom)["id"]);
}


Value* JsonRPC::tryTogetId(Document* dom)
{
	Value* idValue = NULL;

	try
	{
		hasId(dom);
		idValue = getId(dom);
	}
	catch(Error &e)
	{
		throw;
	}
	return idValue;
}



bool JsonRPC::checkJsonRpcVersion(Document* dom)
{
	if(strcmp((*dom)["jsonrpc"].GetString(), JSON_PROTOCOL_VERSION) != 0)
		throw Error(-32002, "Incorrect jsonrpc version. Used version is 2.0");

	return true;
}



bool JsonRPC::isRequest(Document* dom)
{
	bool result = false;

	try
	{
		hasJsonRPCVersion(dom);
		checkJsonRpcVersion(dom);
		hasMethod(dom);
		hasId(dom);
		result = true;
	}
	catch(Error &e)
	{
		result = false;
	}

	return result;
}

bool JsonRPC::isResponse(Document* dom)
{
	bool result = false;

	try
	{
		hasJsonRPCVersion(dom);
		checkJsonRpcVersion(dom);
		hasResultOrError(dom);
		hasId(dom);
		result = true;
	}
	catch(Error &e)
	{
		result = false;
	}
	return result;
}




bool JsonRPC::isError(Document* dom)
{
	bool result = false;

	try
	{
		hasJsonRPCVersion(dom);
		checkJsonRpcVersion(dom);
		hasError(dom);
		hasId(dom);
		result = true;
	}
	catch(Error &e)
	{
		result = false;
	}
	return result;
}



bool JsonRPC::isNotification(Document* dom)
{
	bool result = false;

	try
	{
		hasJsonRPCVersion(dom);
		checkJsonRpcVersion(dom);
		hasMethod(dom);
		hasNoId(dom);
		result = true;
	}
	catch(Error &e)
	{
		result = false;
	}

	return result;
}


bool JsonRPC::hasJsonRPCVersion(Document* dom)
{
	bool result = false;

	try
	{
		if(dom->HasMember("jsonrpc"))
		{
			if((*dom)["jsonrpc"].IsString())
				result = true;
			else
				throw Error(-32001, "Member -jsonrpc- has to be a string.");

		}
		else
			throw Error(-32000, "Member -jsonrpc- is missing.");
	}
	catch(Error &e)
	{
		throw;
	}

	return result;
}


bool JsonRPC::hasMethod(Document* dom)
{
	bool result = false;

	try
	{
		if(dom->HasMember("method"))
		{
			if((*dom)["method"].IsString())
				result = true;
			else
				throw Error(-32011, "Member -method- has to be a string.");
		}
		else
			throw Error(-32010, "Member -method- is missing.");
	}
	catch(Error &e)
	{
		throw;
	}

	return result;
}


bool JsonRPC::hasParams(Document* dom)
{
	bool result = false;

	try
	{
		if(dom->HasMember("params"))
		{
			if((*dom)["params"].IsObject())
				result = true;
			else
			{
				throw Error(-32021, "Member -params- has to be an object.");
			}
		}
		else
			throw Error(-32020, "Member -params- is missing.");
	}
	catch(Error &e)
	{
		throw;
	}

	return result;

}


bool JsonRPC::hasId(Document* dom)
{
	bool result = false;
	Value* id = NULL;

	//TODO: check: normally not NULL, no fractional pars
	try
	{
		if(dom->HasMember("id"))
		{
			id = &(*dom)["id"];

			//id can be either int
			if(id->IsInt())
			{
				result = true;
			}
			else if(id->IsString())
			{
				if(strcmp(id->GetString(), "NULL") == 0)
					throw Error(-32033, "NULL for Member -id- is not allowed.");
				else if(!isNumber(id->GetString()))
					throw Error(-32034, "Member -id- has to be a number.");
				else
					result = true;
			}
			else
				throw Error(-32031, "Member -id- has to be an integer or a string.");
		}
		else
			throw Error(-32030, "Member -id- is missing.");
	}
	catch(Error &e)
	{
		throw;
	}

	return result;
}

bool JsonRPC::isNumber(const char* s)
{
	char* pos = NULL;
	strtol(s, &pos, 10);
	return (*pos == 0);
}


bool JsonRPC::hasNoId(Document* dom)
{
	bool result = false;

	try
	{
		if(!dom->HasMember("id"))
		{
			result = true;
		}
		else
			throw Error(-32032, "Member -id- should not be within Notification.");
			//TODO: freie error nummer wählen( siehe dokument für fehler)
	}
	catch(Error &e)
	{
		throw;
	}
	return result;
}


bool JsonRPC::hasResult(Document* dom)
{
	bool result = false;

	try
	{
		if(dom->HasMember("result"))
			result = true;
			//no checking for type, because the type of result is deetermined by the calling function
		else
			throw Error(-32040, "Member -result- is missing.");
	}
	catch(Error &e)
	{
		throw;
	}

	return result;
}


bool JsonRPC::hasError(Document* dom)
{
	bool result = false;

	try
	{
		if(dom->HasMember("error"))
		{
			if((*dom)["error"].IsObject())
				result = true;
			else
				throw Error( -32051, "Member -error- has to be an object.");
		}
		else
			throw Error(-32050, "Member -error- is missing.");
	}
	catch(Error &e)
	{
		throw;
	}

	return result;
}


bool JsonRPC::hasResultOrError(Document* dom)
{
	bool result = false;

	try
	{
		result = hasResult(dom);
	}
	catch(Error &e)
	{
		result |= hasError(dom);
	}

	return result;
}


Value* JsonRPC::findObjectMember(Value &object, const char* memberName)
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
Value* JsonRPC::findObjectMember(Value &object, const char* memberName, rapidjson::Type shouldBeType)
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


const char* JsonRPC::generateRequest(Value &method, Value &params, Value &id)
{
	Value copyId;

	sBuffer.Clear();
	jsonWriter->Reset(sBuffer);

	if(requestDOM->HasMember("params"))
		requestDOM->RemoveMember("params");

	if(requestDOM->HasMember("id"))
		requestDOM->RemoveMember("id");

	//we always got a method in a request
	requestDOM->RemoveMember("method");
	requestDOM->AddMember("method", method, requestDOM->GetAllocator());


	//params insert as object (params is optional)
	if(&params != NULL)
		requestDOM->AddMember("params", params, requestDOM->GetAllocator());

	//if this is a request notification, id is NULL
	if(&id != NULL)
	{
		copyId.CopyFrom(id, requestDOM->GetAllocator());
		requestDOM->AddMember("id", copyId, requestDOM->GetAllocator());
	}


	requestDOM->Accept(*jsonWriter);

	return sBuffer.GetString();
}




const char* JsonRPC::generateResponse(Value &id, Value &response)
{
	//clear buffer
	Value* oldResult;
	Value copyId;

	sBuffer.Clear();
	jsonWriter->Reset(sBuffer);

	//we always got a method in a request
	responseDOM->RemoveMember("result");
	responseDOM->AddMember("result", response, responseDOM->GetAllocator());

	responseDOM->RemoveMember("id");
	copyId.CopyFrom(id, responseDOM->GetAllocator());
	responseDOM->AddMember("id", copyId, responseDOM->GetAllocator());

	//write DOM to sBuffer
	responseDOM->Accept(*jsonWriter);

	return sBuffer.GetString();
}



const char* JsonRPC::generateResponseError(Value &id, int code, const char* msg)
{
	Value data;

	data.SetString(msg, errorDOM->GetAllocator());
	sBuffer.Clear();
	jsonWriter->Reset(sBuffer);

	//e.g. parsing error, we have no id value.
	if(id.IsInt())
		(*errorDOM)["id"] = id.GetInt();
	else
		(*errorDOM)["id"] = 0;

	(*errorDOM)["error"]["code"] = code;
	(*errorDOM)["error"]["message"] = "Server error";
	(*errorDOM)["error"]["data"].Swap(data);

	errorDOM->Accept(*jsonWriter);

	return sBuffer.GetString();
}


void JsonRPC::generateRequestDOM(Document &dom)
{
	Value id;
	id.SetInt(0);

	dom.SetObject();
	dom.AddMember("jsonrpc", JSON_PROTOCOL_VERSION, dom.GetAllocator());
	dom.AddMember("method", "", dom.GetAllocator());
	dom.AddMember("id", id, dom.GetAllocator());

}


void JsonRPC::generateResponseDOM(Document &dom)
{
	Value id;
	id.SetInt(0);

	dom.SetObject();
	dom.AddMember("jsonrpc", JSON_PROTOCOL_VERSION, dom.GetAllocator());
	dom.AddMember("result", "", dom.GetAllocator());
	dom.AddMember("id", id, dom.GetAllocator());
}


void JsonRPC::generateErrorDOM(Document &dom)
{
	Value id;
	Value error;

	//An error msg is a response with error value but without result value, where error is an object
	dom.SetObject();
	dom.AddMember("jsonrpc", JSON_PROTOCOL_VERSION, dom.GetAllocator());


	error.SetObject();
	error.AddMember("code", 0, dom.GetAllocator());
	error.AddMember("message", "", dom.GetAllocator());
	error.AddMember("data", "", dom.GetAllocator());
	dom.AddMember("error", error, dom.GetAllocator());

	id.SetInt(0);
	dom.AddMember("id", id, dom.GetAllocator());

}

