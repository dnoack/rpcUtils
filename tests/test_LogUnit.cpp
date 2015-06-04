#define protected public
#define private public

#include "LogUnit.hpp"
#include "TestHarness.h"


static LogUnit* logUnit = NULL;



TEST_GROUP(LogUnit)
{
	void setup()
	{
		logUnit = new LogUnit();
	}

	void teardown()
	{
		delete logUnit;
	}
};


TEST(LogUnit, dlog_with_invalidLoglevel)
{
	LogInformation info;
	info.logLevel = 8;
	info.logName = "CppuTest: ";
	const char* msg = "another msg";
	const char* msg2 = "and another one...";

	logUnit->dlog(info, "Testmessage and %s %s", msg, msg2 );
}


TEST(LogUnit, dlog_with_validLoglevel)
{
	LogInformation info;
	info.logLevel = 1;
	info.logName = "CppuTest: ";
	const char* msg = "another msg";
	const char* msg2 = "and another one...";

	logUnit->dlog(info, "Testmessage and %s %s", msg, msg2 );
}


TEST(LogUnit, _log)
{
	string* msg = new string("Testmessage with _log.\n");
	logUnit->_log("CppuTest", msg);
}


TEST(LogUnit, dyn_print)
{
	logUnit->dyn_print("\nTestmessage with dyn_print.\n");
}



TEST(LogUnit, formatString_120)
{
	string* msg = new string("Text of 418 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxx");
	logUnit->setLineSize(120);
	logUnit->formatString(5,& msg);
	printf("\n%s\n", msg->c_str());
	delete msg;
}


TEST(LogUnit, formatString_40)
{
	string* msg = new string("Text of 418 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxx");
	logUnit->setLineSize(40);
	logUnit->formatString(5,& msg);
	printf("\n%s\n", msg->c_str());
	delete msg;
}


TEST(LogUnit, formatString_80)
{
	string* msg = new string("Text of 418 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxxText of 345 charsxx");
	logUnit->formatString(5,& msg);
	printf("\n%s\n", msg->c_str());
	delete msg;
}


TEST(LogUnit, timestamp)
{
	string* msg = new string("");
	logUnit->getTimeStamp(&msg);
	printf("\n%s\n", msg->c_str());
	delete msg;
}


TEST(LogUnit, memoryObjectTest)
{

}



