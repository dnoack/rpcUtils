#ifndef DRIVERINTERFACE_H_
#define DRIVERINTERFACE_H_


#include <map>
#include <list>
#include <cstring>

#include "document.h"
#include "Error.hpp"


using namespace std;
using namespace rapidjson;


/**
 * \interface RPCInterface
 * DriverInterface is a template interface class which should be implemented by all classes which offer rpc functions.
 * The first template parameter has to be a pointer to the class which inherits from DriverInterface itself. The second one has to
 * be the typedef of the functionpointer of functions which should be rpc functions.
 * For example: If the class with the rpc functions is called I2c and the typedef of the functionpointer is
 * typedef bool (I2c::*i2cfptr)(Value&, Value&), it is DriverInterface<I2c*, i2cfptr>. DriverInterface implements a
 * map of the rpc function name and a corresponding functionpointer of type TPointer. All functions which are registered
 * to this map can be called through executeFunction().
 */
template <class TDriver, class TPointer>
class RPCInterface{

	public:
		/** Base-Constructor.
		 * \param derivedClass A pointer to the class which inherits from DriverInterface.
		 */
		RPCInterface(TDriver derivedClass)
		{
			driver = derivedClass;
		};

		/** Base-Destructor.*/
		virtual ~RPCInterface()
		{
			funcMap.clear();
		};

		/**
		 * Searches the internal map of functionspointers for a function with the name
		 * of method. If a function with the corresponding name is found, it is called with
		 * the parameters of params and will save the result in result.
		 * \param method Rapidjson value which contains the methodname of the function which should be executed.
		 * \param params Rapidjson value which contains the corresponding parameters of the function.
		 * \param result Rapidjson value where the result will be saved to.
		 * \return The return value of the called function.
		 * \throws Error When the function was not found within the map.
		 */
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


		/**
		 * Generates a list of all known functionsnames (keys) of the internal map.
		 * \return A pointer to the list with all functionnames as point to std::string*.
		 * \note This function will allocated a new list.
		 */
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

		/** Functor for comparing keys of type const char* in a map.*/
		struct cmp_keys
		{
			/**
			 * \param The string we are looking for.
			 * \param A key of the map we want to compare.
			 * \return True if we got a match, false else.
			 */
			bool operator()(char const* input, char const* key)
			{
				return strcmp(input, key) < 0;
			}
		};


		/*! Map of functionnames (keys) and functionpointers of the derived class (values)*/
		map<const char*, TPointer, RPCInterface::cmp_keys> funcMap;
		/*! Typedef of the functionpointer of the derived class.*/
		TPointer funcP;
		/*! Pointer to the derived class.*/
		TDriver driver;
};



#endif /* PLUGIN_ORIGIN_INCLUDE_PLUGIN_INTERFACE_H_ */
