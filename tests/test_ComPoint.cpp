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

	processInterface->reset();
	delete[] header;
}


TEST(ComPoint, memoryObjectTest)
{

}
