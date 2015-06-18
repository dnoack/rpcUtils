#define protected public
#define private public

#include "ComPointMock.hpp"
#include "ProcessInterfaceMock.hpp"
#include "SocketTestHelper.hpp"
#include "TestHarness.h"


static ComPointMock* comPointMock = NULL;
static ComPoint* comPoint = NULL;
static ProcessInterfaceMock* processInterface = NULL;
static SocketTestHelper* helper = NULL;

TEST_GROUP(ComPoint)
{
	void setup()
	{
		processInterface = new ProcessInterfaceMock();
		helper = new SocketTestHelper(AF_UNIX, SOCK_STREAM, "/tmp/test_com.uds");
		comPointMock = new ComPointMock(helper->getServerSocket(), processInterface, 0);
	}

	void teardown()
	{
		delete processInterface;
		delete comPointMock;
		delete helper;
	}
};


TEST_GROUP(ComPoint_simpleFunctions)
{
	void setup()
	{
		comPoint = new ComPoint();
	}

	void teardown()
	{
		delete comPoint;
	}
};

TEST(ComPoint_simpleFunctions, pushMsgs_popWithOutDelete_checkSize)
{
	RPCMsg* msg = new RPCMsg(0, "And another message");
	comPoint->push_frontReceiveQueue(msg);
	CHECK_EQUAL(1, comPoint->getReceiveQueueSize());
	comPoint->popReceiveQueueWithoutDelete();
	CHECK_EQUAL(0, comPoint->getReceiveQueueSize());
	delete msg;
}



TEST(ComPoint_simpleFunctions, pushMsgs_checkSizeOfQueue_deleteQueue)
{
	RPCMsg* msg = new RPCMsg(0, "This is a testmessage");
	RPCMsg* msg2 = new RPCMsg(0, "Another message");

	comPoint->push_frontReceiveQueue(msg);
	comPoint->push_backReceiveQueue(msg2);
	CHECK_EQUAL(2, comPoint->getReceiveQueueSize());
	comPoint->deleteReceiveQueue();
}


TEST(ComPoint_simpleFunctions, pushMsg_and_popit)
{
	RPCMsg* msg = new RPCMsg(0, "This is a testmessage.");
	comPoint->push_frontReceiveQueue(msg);
	comPoint->popReceiveQueue();
}


TEST(ComPoint_simpleFunctions, makeHeader_negativeValue)
{
	int testValue = -30001;
	char* header = new char[HEADER_SIZE];

	comPoint->createHeader(header, testValue);

	CHECK_EQUAL(0, comPoint->readHeader(header));

	delete[] header;

}


TEST(ComPoint_simpleFunctions, makeHeader_and_convertBack)
{
	char* header = new char[HEADER_SIZE];

	comPoint->createHeader(header, 14);
	CHECK_EQUAL(14, comPoint->readHeader(header));
	memset(header, '\0', HEADER_SIZE);

	comPoint->createHeader(header, 127);
	CHECK_EQUAL(127, comPoint->readHeader(header));
	memset(header, '\0', HEADER_SIZE);

	comPoint->createHeader(header, 144);
	CHECK_EQUAL(144, comPoint->readHeader(header));
	memset(header, '\0', HEADER_SIZE);

	comPoint->createHeader(header, 254);
	CHECK_EQUAL(254, comPoint->readHeader(header));
	memset(header, '\0', HEADER_SIZE);


	comPoint->createHeader(header, 258);
	CHECK_EQUAL(258, comPoint->readHeader(header));
	memset(header, '\0', HEADER_SIZE);

	comPoint->createHeader(header, 24567);
	CHECK_EQUAL(24567, comPoint->readHeader(header));
	memset(header, '\0', HEADER_SIZE);


	delete[] header;

}


TEST(ComPoint_simpleFunctions, convertInt_to_binaryCharArray)
{
	char* header = new char[HEADER_SIZE];
	comPoint->createHeader(header, 1024);
	delete[] header;
}


TEST(ComPoint, waitForFurtherData_and_timeout)
{
	comPointMock->currentSocket = helper->getServerSocket();
	comPointMock->socketToFDSET();
	//will be deleted by Destructor of interface
	comPointMock->msgBuffer = new char[1024];
	CHECK_THROWS(Error, comPointMock->waitForFurtherData());

}


TEST(ComPoint, waitForFurtherData_and_sendit)
{
	comPointMock->currentSocket = helper->getServerSocket();
	comPointMock->socketToFDSET();
	//will be deleted by Destructor of interface
	comPointMock->msgBuffer = new char[1024];

	send(helper->getClientSocket(), "hallo", 5, 0);
	comPointMock->waitForFurtherData();

}



TEST(ComPoint, receiveMsg_everythingSplitted)
{
	char* header = new char[5];
	string* msg = new string("Hallo das ist eine Testnachricht, welche geteilt übertragen wird. Der Header übrigens auch.");
	string subMsg = msg->substr(0, 10);
	string subMsg2 = msg->substr(10, msg->size());
	comPointMock->createHeader(header, msg->size());

	send(helper->getClientSocket(), header, 1, 0);
	send(helper->getClientSocket(), &header[1], 4, 0);
	sleep(1);
	send(helper->getClientSocket(), subMsg.c_str(), subMsg.size(), 0);
	send(helper->getClientSocket(), subMsg2.c_str(), subMsg2.size(), 0);
	sleep(1);


	STRCMP_EQUAL(msg->c_str(), processInterface->getReceivedData()->c_str());

	processInterface->reset();
	delete[] header;
	delete msg;
}




TEST(ComPoint, receiveMsg_splittedHeader)
{
	char* header = new char[5];
	string* msg = new string("Der Header dieser Nachricht wurde in zwei Teilen übertragen.");
	comPointMock->createHeader(header, msg->size());
	send(helper->getClientSocket(), header, 3, 0);
	sleep(1);
	send(helper->getClientSocket(), &header[3], 2, 0);
	send(helper->getClientSocket(), msg->c_str(), msg->size(), 0);
	sleep(1);


	STRCMP_EQUAL(msg->c_str(), processInterface->getReceivedData()->c_str());

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
	comPointMock->createHeader(header, msg->size());
	send(helper->getClientSocket(), header, 5, 0);
	send(helper->getClientSocket(), subMsg.c_str(), subMsg.size(), 0);
	send(helper->getClientSocket(), subMsg2.c_str(), subMsg2.size(), 0);
	sleep(1);


	STRCMP_EQUAL(msg->c_str(), processInterface->getReceivedData()->c_str());

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
	comPointMock->createHeader(header, 5);
	send(helper->getClientSocket(), header, 5, 0);
	send(helper->getClientSocket(), "hallo", 5, 0);

	sleep(1);
	STRCMP_EQUAL("hallo", processInterface->getReceivedData()->c_str());


	processInterface->reset();
	delete[] header;
}


TEST(ComPoint, memoryObjectTest)
{
	sleep(1);
}
