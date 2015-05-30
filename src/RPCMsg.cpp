
#include "RPCMsg.hpp"


RPCMsg::RPCMsg(int sender, string* content)
{
	this->sender = sender;
	this->content = new string(*content);
};


RPCMsg::RPCMsg(int sender, const char* content)
{
	this->sender = sender;
	this->content = new string(content);
};


RPCMsg::RPCMsg(RPCMsg* msg)
{
	this->sender = msg->getSender();
	this->content = new string(msg->getContent()->c_str(), msg->getContent()->size());
}


RPCMsg::~RPCMsg()
{
	delete content;
};
