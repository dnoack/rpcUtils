#ifndef INCLUDE_JSONRPC_HPP_
#define INCLUDE_JSONRPC_HPP_

#include <map>
#include <vector>
#include <list>

/*! Defines the Version of JSON RPC which has to be used.*/
#define JSON_PROTOCOL_VERSION "2.0"

//rapdijson includes
#include "document.h"
#include "writer.h"

using namespace std;
using namespace rapidjson;

/*! Contains information about a functionparameter, which can be used for registering and defining rpc function.
 *\note See rapidjson documentation for the different datatypes like kNumbertype, etc...
 */
struct _param
{
	/*! Name of the parameter.*/
	const char* _name;
	/*! rapdidjson type of the parameter.*/
	rapidjson::Type _type;
};


/**
 * \class JsonRPC
 * JsonRPC is a class for parsing and analyzing JSON RPC 2.0 messages. For parsing JSON, it uses rapidJSON.
 * For JSON RPC it implements different methods to get the fields specified by the JSON RPC 2.0 specification.
 * This class currently support Requests, responses, response errors and notifications of JSON RPC 2.0. BATCH
 * requests are currently NOT supported. Also currently are only named parameters are supported for requests, which
 * means that the member "params" hast to be object withing rapidjson.
 */
class JsonRPC {

	public:

		/**Base-constructor.
		  Generate internal basic DOMs for requests, responses and error responses.*/
		JsonRPC()
		{
			this->currentValue = NULL;

			jsonWriter = new Writer<StringBuffer>(sBuffer);
			requestDOM = new Document();
			responseDOM = new Document();
			errorDOM = new Document();

			generateRequestDOM(*requestDOM);
			generateResponseDOM(*responseDOM);
			generateErrorDOM(*errorDOM);
		};


		/**Base-destructor.*/
		~JsonRPC()
		{
			delete jsonWriter;
			delete requestDOM;
			delete responseDOM;
			delete errorDOM;
		};



		/**
		 * Checks if the json rpc msg member "jsonrpc" has the correct protocol version.
		 * \param dom Document which has a member "jsonrpc".
		 * \throws Error If the json rpc version is not corret.
		 * \return True if the version is correct.
		 */
		bool checkJsonRpcVersion(Document* dom);


		/**
		 * Checks if the DOM has the required members specified in the specification for requests and
		 * as far as possible checks if they are valid. Required members are: jsonrpc, method and id. Member
		 * params is optional.
		 * \param dom A instance of Document which should be analyzed for a valid request.
		 * \return True if the DOM contains a valid json rpc request, false otherwise.
		 */
		bool isRequest(Document* dom);


		/**
		 * Checks if the DOM has the required members specified in the specification for responses and as
		 * far as possible checks if they are valid. Required members are: jsonrpc, result and id.
		 * \param dom A instance of Document which should be analyzed for a valid response.
		 * \return True if the DOM contains a valid json rpc response, false otherwise.
		 */
		bool isResponse(Document* dom);


		/**
		 * Checks if the DOM has the required members specified in the specification for error responses and
		 * as far as possible checks if they are valid. Required members are: jsonrpc, error, id.
		 * \param dom A instance of Document which should be analyzed for a valid error response.
		 * \return True if the DOM contains a valid json rpc error response, false otherwise.
		 */
		bool isError(Document* dom);


		/**
		 * Checks if the DOM has the required members specified in the specification for a notification and
		 * as far as possible checks if they are valid. Required members are: jsonrpc, method. Optional: params.
		 * Member which are not allowed: id.
		 *\param dom A instance of Document which should be analyzed for a valid notification.
		 *\return True if the DOM contains a valid json rpc notification, false otherwise.
		 */
		bool isNotification(Document* dom);

		/**
		 * Parses a JSON string with the rapidJSOn parser into a DOM.
		 * \param dom Instance of Document where the JSON message should be parsed to.
		 * \param msg The message that should be parsed.
		 * \throws Error If there is a parsing error.
		 */
		void parse(Document* dom, string* msg);

		/**
		 * \param dom Document for searching the member "params"
		 * \return The value of the member named "params" as rapidjson::Value
		 * \note Use this function with care, if no member "params" is within the DOM this can cause critical errors.
		 */
		Value* getParams(Document* dom);


		/**
		 * Looks into the DOM if there is a member named "params" and returns it.
		 * \param dom Document for searching the member "params".
		 * \return The value of the member "params" as rapidjson::Value.
		 * \throws Error If no member "params" is found within dom.
		 */
		Value* tryTogetParams(Document* dom);


		/**
		 * \param dom Document for searching a specific parameter, which is a member of the value "params".
		 * \return The value of the specified parameter.
		 * \note Use this function with care, if no member "params" is within the DOM, or no the specified parameter is
		 * not part of params, this will cause critical errors.
		 */
		Value* getParam(Document* dom, const char* name);


		/**
		 * Looks into the DOM if there is a member named "params" and if it contains the specified member.
		 * \param dom Document for searching specified parameter aka. the specified member of the member params.
		 * \return The value of the specified parameter.
		 * \throws Error If no member "params" is found or if params does not contain the specified parameter.
		 */
		Value* tryTogetParam(Document* dom, const char* name);


		/**
		 * \param dom Document for searching the member "result"
		 * \return The value of the member named "result" as rapidjson::Value
		 * \note Use this function with care, if no member "result" is within the DOM this can cause critical errors.
		 */
		Value* getResult(Document* dom);


		/**
		 * Looks into the DOM if there is a member named "result" and returns it.
		 * \param dom Document for searching the member "result".
		 * \return The value of the member "result" as rapidjson::Value.
		 * \throws Error If no member "result" is found within dom.
		 */
		Value* tryTogetResult(Document* dom);


		/**
		 * \param dom Document for searching the member "method"
		 * \return The value of the member named "method" as rapidjson::Value
		 * \note Use this function with care, if no member "method" is within the DOM this can cause critical errors.
		 */
		Value* getMethod(Document* dom);


		/**
		 * Looks into the DOM if there is a member named "method" and returns it.
		 * \param dom Document for searching the member "method".
		 * \return The value of the member "method" as rapidjson::Value.
		 * \throws Error If no member "method" is found within dom.
		 */
		Value* tryTogetMethod(Document* dom);


		/**
		 * \param dom Document for searching the member "id"
		 * \return The value of the member named "id" as rapidjson::Value
		 * \note Use this function with care, if no member "id" is within the DOM this can cause critical errors.
		 */
		Value* getId(Document* dom);


		/**
		 * Looks into the DOM if there is a member named "id" and returns it.
		 * \param dom Document for searching the member "id".
		 * \return The value of the member "id" as rapidjson::Value.
		 * \throws Error If no member "id" is found within dom.
		 */
		Value* tryTogetId(Document* dom);


		/**
		 * Checks the DOM if there is a member "jsonrpc" and if its value is a string.
		 * \param dom The document which should be searched for "jsonrpc".
		 * \return True if the DOM contains a valid member "jsonrpc"
		 * \throws Error If the value of "jsonrpc" is no string or if "jsonrpc" is completely missing.
		 * \note This method does not check the value for the correct version.
		 */
		bool hasJsonRPCVersion(Document* dom);


		/**
		 * Checks the DOM if there is a member "method" and if its value is a string.
		 * \param dom The document which should be searched for "method".
		 * \return True if the DOM contains a valid member "method"
		 * \throws Error If the value of "method" is no string or if "method" is completely missing.
		 */
		bool hasMethod(Document* dom);


		/**
		 * Checks the DOM if there is a member "params" and if its value is a object.
		 * \param dom The document which should be searched for "params".
		 * \return True if the DOM contains a valid member "params"
		 * \throws Error If the value of "params" is no object or if "params" is completely missing.
		 */
		bool hasParams(Document* dom);


		/**
		 * Checks the DOM if there is a member "id" and if its value is a non fractional number.
		 * \param dom The document which should be searched for "id".
		 * \return True if the DOM contains a valid member "id"
		 * \throws Error If the value of "id" is not a non fractional number or if "id" is completely missing.
		 */
		bool hasId(Document* dom);

		/**
		 * Inverse to hasId(Document* dom). Checks the DOm if there NO member "id".
		 * \param dom The document which should be searched for "id".
		 * \return True if there is NO DOM.
		 * \throws Error If the DOM contains a member "id".
		 */
		bool hasNoId(Document* dom);

		/**
		 * Checks the DOM if there is a member "result".
		 * \param dom The document which should be searched for "result".
		 * \return True if the DOM contains a valid member "result"
		 * \throws Error If the value "result" is completely missing.
		 */
		bool hasResult(Document* dom);


		/**
		 * Checks the DOM if there is a member "error" and if its value is a object.
		 * \param dom The document which should be searched for "error".
		 * \return True if the DOM contains a valid member "error"
		 * \throws Error If the value of "error" is no object or if "error" is completely missing.
		 */
		bool hasError(Document* dom);


		/**
		 * Just combines hasResult and hasError with a logical OR.
		 * \param dom The document which should be searched for "error" and "result"
		 * \return True if the DOM contains a valid member "error" or "result".
		 * \throws Error If no member "error" and no "member" result was found.
		 */
		bool hasResultOrError(Document* dom);


		/**
		 * Checks if a string could be converted into a valid number.
		 * \param s The string that should be analyzed.
		 * \return True if the string contains ONLY a number, false otherwise.
		 */
		bool isNumber(const char* s);


		/**
		 * Searches a Value, which should be an object, for a specified member.
		 * \param object The rapidJson::Value which should be searched for the specified member.
		 * \param memberName Name of the searched member.
		 * \return The searched member as rapidjson::Value.
		 * \throws Error If the parameter object was no object or if the member was not found.
		 */
		Value* findObjectMember(Value &object, const char* memberName);


		/**
		 * Like above but with additional typecheck.
		 * \param object The rapidJson::Value which should be searched for the specified member.
		 * \param memberName Name of the searched member.
		 * \param shoudBeType The rapidjson::Type which the member should have.
		 * \return The searched member as rapidjson::Value.
		 * \throws Error If the parameter object was no object or if the member was not found or has the wrong type.
		 */
		Value* findObjectMember(Value &object, const char* memberName, rapidjson::Type shouldBeType);

		/**
		 * Generates a valid JSON RPC request message.
		 * \param method Value of member method.
		 * \param params Value of member params, this MUST BE a object.
		 * \param id Value of member id, this MUST BE NOT NULL and NOT 0.
		 * \return JSON RPC request as string.
		 */
		const char* generateRequest(Value &method, Value &params, Value &id);


		/**
		 * Generates a valid JSON RPC response message.
		 * \param id Value of member id, this MUST BE NOT NULL and NOT 0.
		 * \param response Value of member result.
		 * \return JSON RPC response as string.
		 */
		const char* generateResponse(Value &id, Value &response);


		/**
		 * Generates a valid JSON RPC error response message.
		 * \param id Value of member id, this can be 0 for parse errors.
		 * \param code Value of member code within the member error
		 * \param msg Value of member data within the member error
		 * \return JSON RPC error response as string.
		 * \note Member message within the member error will be set to "Server error"
		 */
		const char* generateResponseError(Value &id, int code, const char* msg);


		/** \return DOm for generating requests.*/
		Document* getRequestDOM() { return this->requestDOM;}
		/** \return DOM for generating responses.*/
		Document* getResponseDOM() { return this->responseDOM;}
		/** \return DOM for generatin error responses.*/
		Document* getErrorDOM(){ return this->errorDOM;}


	private:

		/*! OutputBuffer for writing JSON with rapidJSON parser.*/
		StringBuffer sBuffer;
		/*! Class for writing a DOM to a JSON message as string.*/
		Writer<StringBuffer>* jsonWriter;

		/*! DOM for generating JSON RPC requests.*/
		Document* requestDOM;
		/*! DOM for generating JSON RPC responses.*/
		Document* responseDOM;
		/*! DOM for generating JSON RPC error responses.*/
		Document* errorDOM;

		/*! Value from DOM which is currently examined.*/
		Value currentValue;

		/**
		 * Pre-generates a DOM for JSON RPC request messages.
		 *\param dom A instance of Document (Constructor uses requestDOM).
		 */
		void generateRequestDOM(Document &dom);


		/**
		 * Pre-generates a DOM for JSON RPC response messages.
		 * \param dom A instance of Document (Constructor uses responseDOM).
		 */
		void generateResponseDOM(Document &dom);


		/**
		 * Pre generates a DOM for JSON RPC error response messages.
		 *\param dom A instance of Documnet ( Constructor uses errorDOM).
		 */
		void generateErrorDOM(Document &dom);

};


#endif /* INCLUDE_JSONRPC_HPP_ */
