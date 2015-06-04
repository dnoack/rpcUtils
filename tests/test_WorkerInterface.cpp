#define protected public
#define private public

#include "WorkerInterface.hpp"
#include "WorkerInterfaceMock.hpp"
#include "SocketTestHelper.hpp"
#include "TestHarness.h"


static WorkerInterfaceMock* interface = NULL;

TEST_GROUP(WorkerInterface)
{
	void setup()
	{
		interface = new WorkerInterfaceMock();
	}

	void teardown()
	{
		delete interface;
	}
};


TEST(WorkerInterface, waitForFurtherData_and_timeout)
{
	SocketTestHelper* helper = new SocketTestHelper(AF_UNIX, SOCK_STREAM, "/tmp/test_com.uds");
	interface->currentSocket = helper->getServerSocket();
	interface->socketToFDSET();
	//will be deleted by Destructor of interface
	interface->msgBuffer = new char[1024];
	CHECK_THROWS(Error, interface->waitForFurtherData());
	delete helper;
}


TEST(WorkerInterface, waitForFurtherData_and_sendit)
{
	SocketTestHelper* helper = new SocketTestHelper(AF_UNIX, SOCK_STREAM, "/tmp/test_com.uds");
	interface->currentSocket = helper->getServerSocket();
	interface->socketToFDSET();
	//will be deleted by Destructor of interface
	interface->msgBuffer = new char[1024];

	send(helper->getClientSocket(), "hallo", 5, 0);
	interface->waitForFurtherData();

	delete helper;
}


TEST(WorkerInterface, pushMsgs_popWithOutDelete_checkSize)
{
	RPCMsg* msg = new RPCMsg(0, "And another message");
	interface->push_frontReceiveQueue(msg);
	CHECK_EQUAL(1, interface->getReceiveQueueSize());
	interface->popReceiveQueueWithoutDelete();
	CHECK_EQUAL(0, interface->getReceiveQueueSize());
	delete msg;
}



TEST(WorkerInterface, pushMsgs_checkSizeOfQueue_deleteQueue)
{
	RPCMsg* msg = new RPCMsg(0, "This is a testmessage");
	RPCMsg* msg2 = new RPCMsg(0, "Another message");

	interface->push_frontReceiveQueue(msg);
	interface->push_backReceiveQueue(msg2);
	CHECK_EQUAL(2, interface->getReceiveQueueSize());
	interface->deleteReceiveQueue();
}


TEST(WorkerInterface, pushMsg_and_popit)
{
	RPCMsg* msg = new RPCMsg(0, "This is a testmessage.");
	interface->push_frontReceiveQueue(msg);
	interface->popReceiveQueue();
}


TEST(WorkerInterface, makeHeader_negativeValue)
{
	int testValue = -30001;
	char* header = new char[HEADER_SIZE];

	interface->createHeader(header, testValue);

	CHECK_EQUAL(0, interface->readHeader(header));

	delete[] header;

}


TEST(WorkerInterface, makeHeader_and_convertBack)
{
	char* header = new char[HEADER_SIZE];

	interface->createHeader(header, 14);
	CHECK_EQUAL(14, interface->readHeader(header));
	memset(header, '\0', HEADER_SIZE);

	interface->createHeader(header, 127);
	CHECK_EQUAL(127, interface->readHeader(header));
	memset(header, '\0', HEADER_SIZE);

	interface->createHeader(header, 144);
	CHECK_EQUAL(144, interface->readHeader(header));
	memset(header, '\0', HEADER_SIZE);

	interface->createHeader(header, 254);
	CHECK_EQUAL(254, interface->readHeader(header));
	memset(header, '\0', HEADER_SIZE);


	interface->createHeader(header, 258);
	CHECK_EQUAL(258, interface->readHeader(header));
	memset(header, '\0', HEADER_SIZE);

	interface->createHeader(header, 24567);
	CHECK_EQUAL(24567, interface->readHeader(header));
	memset(header, '\0', HEADER_SIZE);


	delete[] header;

}


TEST(WorkerInterface, convertInt_to_binaryCharArray)
{
	char* header = new char[HEADER_SIZE];
	interface->createHeader(header, 1024);
	delete[] header;
}


//Creates and destroys the object and checks memory
TEST(WorkerInterface, memoryObjectTest)
{
	CHECK_EQUAL(false, interface->isDeletable());
}
