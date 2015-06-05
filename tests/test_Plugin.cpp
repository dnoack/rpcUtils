#define private public

#include "Plugin.hpp"
#include "TestHarness.h"



TEST_GROUP(Plugin)
{
	void setup()
	{

	}

	void teardown()
	{

	}
};

TEST(Plugin, addMethods)
{
	Plugin* plugin = new Plugin("Aardvark", 1, "/tmp/aardvark.ipc");
	string* method1 = new string("add");
	string* method2 = new string("sub");
	list<string*>* methodList = NULL;

	plugin->addMethod(method1);
	plugin->addMethod(method2);

	methodList = plugin->getMethods();
	CHECK_EQUAL(2, methodList->size());

	delete plugin;
}


TEST(Plugin, checkMemory)
{
	Plugin* plugin1 = new Plugin("Aardvark", 1, "/tmp/aardvark.ipc");
	Plugin* plugin2 = new Plugin(plugin1);


	delete plugin1;
	delete plugin2;
}
