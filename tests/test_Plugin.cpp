#define private public

#include "PluginInfo.hpp"
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
	PluginInfo* plugin = new PluginInfo("Aardvark", 1, "/tmp/aardvark.ipc");
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
	PluginInfo* plugin1 = new PluginInfo("Aardvark", 1, "/tmp/aardvark.ipc");
	PluginInfo* plugin2 = new PluginInfo(plugin1);


	delete plugin1;
	delete plugin2;
}
