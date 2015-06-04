
#include "RPCMsg.hpp"


RPCMsg::RPCMsg(int sender, string* content)
{
	this->sender = sender;
	this->content = content;
	this->jsonRpcId = 0;
};


RPCMsg::RPCMsg(int sender, const char* content)
{
	this->sender = sender;
	this->content = new string(content);
	this->jsonRpcId = 0;
};


RPCMsg::RPCMsg(RPCMsg* msg)
{
	this->sender = msg->getSender();
	this->content = new string(msg->getContent()->c_str(), msg->getContent()->size());
	this->jsonRpcId = msg->getJsonRpcId();
}


RPCMsg::~RPCMsg()
{
	delete content;
};
