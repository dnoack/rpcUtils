
#ifndef INCLUDE_ERROR_HPP_
#define INCLUDE_ERROR_HPP_

#define LINEBUFFER_SIZE 64

#include <exception>
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <cstring>

using namespace std;


class Error : exception{


	public:
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


		Error(const char* msg)
		{
			this->errorCode = 0;
			this->exMsg = new string(msg);
		}

		Error(int errorCode, const char* msg)
		{
			this->errorCode = errorCode;
			this->exMsg = new string(msg);
		}

		Error(int errorCode, const char* msg, const char* errnoMsg)
		{
			this->errorCode = errorCode;
			this->exMsg = new string(msg);
			this->exMsg->append(" - errno: ");
			this->exMsg->append(errnoMsg);
		}


		Error(string* msg)
		{
			this->errorCode = 0;
			this->exMsg = new string(*msg);
		}


		~Error() throw()
		{
			delete exMsg;
		}


		const char* get() const{return exMsg->c_str();}
		string* getString(){return this->exMsg;}
		int getErrorCode(){return this->errorCode;}

		void append(string* msg)
		{
			this->exMsg->append(": ");
			this->exMsg->append(*msg);
		}

	private:
		string* exMsg;
		int errorCode;
		char lineBuffer[LINEBUFFER_SIZE];

};

#endif /* INCLUDE_ERROR_HPP_ */
