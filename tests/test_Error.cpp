
#define private public
#include "Error.hpp"
#include "TestHarness.h"



TEST_GROUP(Error)
{
	void setup()
	{

	}

	void teardown()
	{

	}
};


TEST(Error, constructor_with_string)
{
	string* msg = new string("Errornachricht als string.");
	Error* error = new Error(msg);

	STRCMP_EQUAL(msg->c_str(), error->get());
	delete error;
	delete msg;
}


TEST(Error, constructor_wirht_errorCode_and_msg_and_info)
{
	const char* msg = "This is a error message without extra information";
	int errorNumber = -3005;
	const char* info = "This could be a errno information.";
	Error* error = new Error(errorNumber, msg, info);

	STRCMP_EQUAL("This is a error message without extra information - errno: This could be a errno information." , error->get());
	CHECK_EQUAL(errorNumber, error->getErrorCode());
	delete error;
}



TEST(Error, constructor_with_errorCode_and_msg)
{
	const char* msg = "This is a error message without extra information";
	int errorNumber = -3005;
	Error* error = new Error(errorNumber, msg);
	STRCMP_EQUAL(msg, error->get());
	CHECK_EQUAL(errorNumber, error->getErrorCode());
	delete error;
}



TEST(Error, constructor_with_msg)
{
	const char* msg = "This is a error message without extra information";
	Error* error = new Error(msg);
	STRCMP_EQUAL(msg, error->get());
	delete error;
}


TEST(Error, constructor_with_file_and_line)
{
	Error* error = new Error("This is an error message ", "/tmp/error.c", 234);
	STRCMP_EQUAL("This is an error message An error was thrown in file: /tmp/error.c at line: 234", error->get());
	delete error;

}
