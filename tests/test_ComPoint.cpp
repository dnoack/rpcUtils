#define protected public
#define private public

#include "ComPoint.hpp"
#include "ProcessInterfaceMock.hpp"
#include "SocketTestHelper.hpp"
#include "TestHarness.h"


static ComPoint* comPoint = NULL;
static ProcessInterfaceMock* processInterface = NULL;
static SocketTestHelper* helper = NULL;

TEST_GROUP(ComPoint)
{
	void setup()
	{
		processInterface = new ProcessInterfaceMock();
		helper = new SocketTestHelper(AF_UNIX, SOCK_STREAM, "/tmp/test_com.uds");
		comPoint = new ComPoint(helper->getServerSocket(), processInterface, 0);
	}

	void teardown()
	{
		delete processInterface;
		delete comPoint;
		delete helper;
	}
};



TEST(ComPoint, receiveMsg_everythingSplitted)
{
	char* header = new char[5];
	string* msg = new string("Hallo das ist eine Testnachricht, welche geteilt übertragen wird. Der Header übrigens auch.");
	string subMsg = msg->substr(0, 10);
	string subMsg2 = msg->substr(10, msg->size());
	comPoint->createHeader(header, msg->size());

	send(helper->getClientSocket(), header, 1, 0);
	send(helper->getClientSocket(), &header[1], 4, 0);
	sleep(1);
	send(helper->getClientSocket(), subMsg.c_str(), subMsg.size(), 0);
	send(helper->getClientSocket(), subMsg2.c_str(), subMsg2.size(), 0);
	sleep(1);

	if(processInterface->receivedMsg == NULL)
	{
		FAIL("Could not receive a correct Message, maybe a timeout happened.");
	}
	else
		STRCMP_EQUAL(msg->c_str(), processInterface->receivedMsg->getContent()->c_str());

	processInterface->reset();
	delete[] header;
	delete msg;
}




TEST(ComPoint, receiveMsg_splittedHeader)
{
	char* header = new char[5];
	string* msg = new string("Der Header dieser Nachricht wurde in zwei Teilen übertragen.");
	comPoint->createHeader(header, msg->size());
	send(helper->getClientSocket(), header, 3, 0);
	sleep(1);
	send(helper->getClientSocket(), &header[3], 2, 0);
	send(helper->getClientSocket(), msg->c_str(), msg->size(), 0);
	sleep(1);

	if(processInterface->receivedMsg == NULL)
	{
		FAIL("Could not receive a correct Message, maybe a timeout happened.");
	}
	else
		STRCMP_EQUAL(msg->c_str(), processInterface->receivedMsg->getContent()->c_str());

	processInterface->reset();
	delete[] header;
	delete msg;

}


TEST(ComPoint, receiMsg_splittedMsg)
{
	char* header = new char[5];
	string* msg = new string("Hallo das ist eine Testnachricht, welche geteilt übertragen wird.");
	string subMsg = msg->substr(0, 10);
	string subMsg2 = msg->substr(10, msg->size());
	comPoint->createHeader(header, msg->size());
	send(helper->getClientSocket(), header, 5, 0);
	send(helper->getClientSocket(), subMsg.c_str(), subMsg.size(), 0);
	send(helper->getClientSocket(), subMsg2.c_str(), subMsg2.size(), 0);
	sleep(1);

	if(processInterface->receivedMsg == NULL)
	{
		FAIL("Could not receive a correct Message, maybe a timeout happened.");
	}
	else
		STRCMP_EQUAL(msg->c_str(), processInterface->receivedMsg->getContent()->c_str());

	processInterface->reset();
	delete[] header;
	delete msg;
}


TEST(ComPoint, receiveMsgwithNoHeader)
{
	send(helper->getClientSocket(), "hallo", 5, 0);
	sleep(5);
	processInterface->reset();
}


TEST(ComPoint, receiveMsg)
{
	char* header = new char[5];
	comPoint->createHeader(header, 5);
	send(helper->getClientSocket(), header, 5, 0);
	send(helper->getClientSocket(), "hallo", 5, 0);
	sleep(2);
	CHECK(processInterface->msgReceived);
	STRCMP_EQUAL("hallo", processInterface->receivedMsg->getContent()->c_str());

	processInterface->reset();
	delete[] header;
}


TEST(ComPoint, memoryObjectTest)
{

}
