/*
 * RPCMsg.hpp
 *
 *  Created on: 30.05.2015
 *      Author: Dave
 */

#ifndef RPCMSG_HPP_
#define RPCMSG_HPP_

#include <string>

#define TCP_SIDE 0
#define IPC_SIDE 1

using namespace std;

class ComPoint;

class RPCMsg{

	public:
		RPCMsg()
		{
			this->comPoint = NULL;
			this->content = NULL;
			this->jsonRpcId = 0;
		};

		RPCMsg(ComPoint* comPoint, string* content)
		{
			this->comPoint = comPoint;
			this->content = content;
			this->jsonRpcId = 0;
		};

		RPCMsg(ComPoint* comPoint, const char* content)
		{
			this->comPoint = comPoint;
			this->content = new string(content);
			this->jsonRpcId = 0;
		};

		RPCMsg(RPCMsg* rpcMsg)
		{
			this->comPoint = rpcMsg->getComPoint();
			this->content = new string(rpcMsg->getContent()->c_str(), rpcMsg->getContent()->size());
			this->jsonRpcId = rpcMsg->getJsonRpcId();
		};

		virtual ~RPCMsg()
		{
			delete content;
		};


		string* getContent(){return this->content;}
		ComPoint* getComPoint(){return this->comPoint;}
		bool isOriginTcp();

		int getJsonRpcId(){return this->jsonRpcId;}
		void setJsonRpcId(int jsonRpcId){this->jsonRpcId = jsonRpcId;}


	protected:
		/*! This is the proper message, hopefully containing a valid json rpc message.*/
		string* content;
		ComPoint* comPoint;
		int jsonRpcId;
};


#endif /* RPCMSG_HPP_ */
