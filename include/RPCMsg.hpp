/*
 * RPCMsg.hpp
 *
 *  Created on: 30.05.2015
 *      Author: Dave
 */

#ifndef RPCMSG_HPP_
#define RPCMSG_HPP_

#include <string>

using namespace std;

class RPCMsg{

	public:
		RPCMsg(int sender, string* content);

		RPCMsg(int sender, const char* content);


		/**
		 * Copy constructor.
		 */
		RPCMsg(RPCMsg* msg);

		virtual ~RPCMsg();


		/**
		 * Overloaded operator == for comparing two messages.
		 * \msg2 The other message which will be compared with the instance of this one.
		 * \return Returns true if both messages are equal, false instead.
		 */
		bool operator==(RPCMsg* msg2)
		{
			string* content1 = this->getContent();
			string* content2 = msg2->getContent();

			if (content1->compare(*content2) && this->getSender() == msg2->getSender())
				return true;
			else
				return false;
		}


		/**
		 * \return Sender id of this message.
		 */
		int getSender(){return this->sender;}

		/**
		 * \return Content of this message as a pointer to std::string.
		 */
		string* getContent(){return this->content;}


	private:
		/*! Sender id which is 0 for tcp clients and greater 0 for plugins.*/
		int sender;
		/*! This is the proper message, hopefully containing a valid json rpc message.*/
		string* content;


};



#endif /* RPCMSG_HPP_ */
