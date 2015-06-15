
#ifndef INCLUDE_ERROR_HPP_
#define INCLUDE_ERROR_HPP_

/*! Buffer for using the __LINE__ Macro.*/
#define LINEBUFFER_SIZE 64
/*! Limit of string size while using variable constructors.*/
#define ARGUMENT_BUFFER_SIZE 2048

#include <exception>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <iostream>
#include <stdio.h>
#include <cstring>

using namespace std;


/**
 * \class Error
 * Error is a simple exception class which inherits from exception itself and can transport some
 * additional information about a thrown exception.
 */
class Error : exception{


	public:

		/**
		 * Constructor with additional information about the file and line where the exception was thrown.
		 * \param msg The exception message itself.
		 * \param file Use macro __FILE__ as parameter to get the actual file where the exception was thrown.
		 * \param line Use macro __LINE__ as parameter to get the actual line where the exception was thrown.
		 */
		Error(const char* msg, const char* file, int line)
		{
			this->exMsg = new string(msg);
			this->errorCode = 0;
			memset(lineBuffer, '\0', LINEBUFFER_SIZE);

			exMsg->append("An error was thrown in file: ");
			exMsg->append(file);
			exMsg->append(" at line: ");
			snprintf(lineBuffer, LINEBUFFER_SIZE, "%d", line);
			exMsg->append(lineBuffer);
		}


		/** Simple constructor where Error contains just a message.
		 * \param msg The exception message itself.
		 * \note Parameter msg will internally copied.
		 */
		Error(string* msg)
		{
			this->errorCode = 0;
			this->exMsg = new string(*msg);
		}


		/**
		 * Constructor for nesting an arbitrary number of strings into the exception information.
		 * You can call this constructor like printf().
		 * For example: new Error("%s : %s : %s", ps1, ps2 ,ps2); where psX are pointer to strings.
		 * \param msg A string which can contain escape chapters for other strings or variables.
		 * \note The maximum length of all strings together cannot exceed ARGUMENT_BUFFER_SIZE.
		 */
		Error(const char* msg, ...)
		{
			va_list arguments;
			memset(argumentBuffer, '\0', ARGUMENT_BUFFER_SIZE);
			this->errorCode = 0;

			va_start(arguments, msg);
			vsnprintf(argumentBuffer, ARGUMENT_BUFFER_SIZE, msg, arguments);
			this->exMsg = new string(argumentBuffer);
			va_end(arguments);
		}

		/**
		 * Constructor for nesting an arbitrary number of strings into the exception information.
		 * You can call this constructor like printf(). Additionally a erroCode will be need which is related to
		 * the json rpc error code.
		 * For example: new Error(-3005, "%s : %s : %s", ps1, ps2 ,ps2); where psX are pointer to strings.
		 * \param errorCode A error-code which can be represented as json rpc error code.
		 * \param msg A string which can contain escape chapters for other strings or variables.
		 * \note The maximum length of all strings together cannot exceed ARGUMENT_BUFFER_SIZE.
		 */
		Error(int errorCode, const char* msg, ...)
		{
			va_list arguments;
			memset(argumentBuffer, '\0', ARGUMENT_BUFFER_SIZE);
			this->errorCode = errorCode;

			va_start(arguments, msg);
			vsnprintf(argumentBuffer, ARGUMENT_BUFFER_SIZE, msg, arguments);
			this->exMsg = new string(argumentBuffer);
			va_end(arguments);
		}

		/**
		 * Base-Destructor.
		 * Internal exception message will be deallocated.
		 */
		~Error() throw()
		{
			delete exMsg;
		}

		/** \return The internal exception information.*/
		const char* get() const{return exMsg->c_str();}
		/** \return The internal exception information.*/
		string* getString(){return this->exMsg;}
		/** \return Errorcode of the thrown exception. This is 0 if not set.*/
		int getErrorCode(){return this->errorCode;}

		/**
		 * After constructing an instance of Error we can use this function to append
		 * some additional information to the internal exception information. Every additional
		 * information will be added with a colon and a blank space.
		 * \param msg The information which should be added.
		 */
		void append(string* msg)
		{
			this->exMsg->append(": ");
			this->exMsg->append(*msg);
		}

	private:
		/*! Saves the information about the exception.*/
		string* exMsg;
		/*! Contains a error-code which can be represented as json rpc error code.*/
		int errorCode;
		/*! Can contain a string representing the line where the exception was thrown.*/
		char lineBuffer[LINEBUFFER_SIZE];
		/*! Can contain a string representing the file where the exception was thrown.*/
		char argumentBuffer[ARGUMENT_BUFFER_SIZE];
};

#endif /* INCLUDE_ERROR_HPP_ */
