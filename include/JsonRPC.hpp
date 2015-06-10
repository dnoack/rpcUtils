
#ifndef INCLUDE_JSONRPC_HPP_
#define INCLUDE_JSONRPC_HPP_

#define JSON_PROTOCOL_VERSION "2.0"

#include <map>
#include <vector>
#include <list>


//rapdijson includes
#include "document.h"
#include "writer.h"


using namespace std;
using namespace rapidjson;

struct _param
{
	const char* _name;
	rapidjson::Type _type;
};



class JsonRPC {

	public:


		JsonRPC()
		{
			this->currentValue = NULL;
			this->result = NULL;
			this->error = NULL;

			jsonWriter = new Writer<StringBuffer>(sBuffer);
			requestDOM = new Document();
			responseDOM = new Document();
			errorDOM = new Document();

			generateRequestDOM(*requestDOM);
			generateResponseDOM(*responseDOM);
			generateErrorDOM(*errorDOM);
		};



		~JsonRPC()
		{
			delete jsonWriter;
			delete requestDOM;
			delete responseDOM;
			delete errorDOM;
		};



		/**
		 * Checks if the json rpc msg member "jsonrpc" has the correct protocol version.
		 */
		bool checkJsonRpcVersion(Document* dom);


		/**
		 * Checks if there is a member named "id". If not the msg is assumed to be
		 * a notification. If "id" is existing, the msg is a request. For checking the other
		 * mandatory request fields, you need to call checkJsonRpc_RequestFormat().
		 */
		bool isRequest(Document* dom);
		bool isResponse(Document* dom);
		bool isError(Document* dom);
		bool isNotification(Document* dom);


		void parse(Document* dom, string* msg);


		Value* getParams(Document* dom);
		Value* tryTogetParams(Document* dom);

		Value* getParam(Document* dom, const char* name);
		Value* tryTogetParam(Document* dom, const char* name);


		Value* getResult(Document* dom);
		Value* tryTogetResult(Document* dom);

		Value* getMethod(Document* dom);
		Value* tryTogetMethod(Document* dom);

		Value* getId(Document* dom);
		Value* tryTogetId(Document* dom);


		//true if field is there, otherwhise throws Error exception
		bool hasJsonRPCVersion(Document* dom);
		bool hasMethod(Document* dom);
		bool hasParams(Document* dom);
		bool hasId(Document* dom);
		bool hasNoId(Document* dom);
		bool hasResult(Document* dom);
		bool hasError(Document* dom);
		bool hasResultOrError(Document* dom);

		Value* findObjectMember(Value &object, const char* memberName);
		Value* findObjectMember(Value &object, const char* memberName, rapidjson::Type shouldBeType);

		const char* generateRequest(Value &method, Value &params, Value &id);
		const char* generateResponse(Value &id, Value &response);
		const char* generateResponseError(Value &id, int code, const char* msg);

		Document* getRequestDOM() { return this->requestDOM;}
		Document* getResponseDOM() { return this->responseDOM;}
		Document* getErrorDOM(){ return this->errorDOM;}


	private:

		//compare functor handels char* compare for the map
		struct cmp_keys
		{
			bool operator()(char const* input, char const* key)
			{
				return strcmp(input, key) < 0;
			}
		};

		//for output
		StringBuffer sBuffer;
		Writer<StringBuffer>* jsonWriter;


		//prepared DOMs for faster request/response generation
		Document* requestDOM;
		Document* responseDOM;
		Document* errorDOM;

		//Value from dom which is currently examined
		Value currentValue;

		//result from the processed function
		Value result;
		const char* error;


		void generateRequestDOM(Document &dom);
		void generateResponseDOM(Document &dom);
		void generateErrorDOM(Document &dom);

};



#endif /* INCLUDE_JSONRPC_HPP_ */
