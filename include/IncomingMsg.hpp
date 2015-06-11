/*
 * IncommingMsg.hpp
 *
 *  Created on: 11.06.2015
 *      Author: dave2
 */

#ifndef INCLUDE_INCOMINGMSG_HPP_
#define INCLUDE_INCOMINGMSG_HPP_

#include "stdio.h"
#include "RPCMsg.hpp"

using namespace std;

class ComPoint;

class IncomingMsg : public RPCMsg{

	public:
		IncomingMsg(ComPoint* origin, string* content )
		{
			//incommingMsg content will NOT be copied
			this->content = content;;
			this->comPoint = origin;
		}

		IncomingMsg(ComPoint* origin, const char* content )
		{
			this->content = new string(content);
			this->comPoint = origin;
		}

		virtual ~IncomingMsg(){}

		string* getContent(){return this->content;}
		ComPoint* getOrigin(){return this->comPoint;}

};



#endif /* INCLUDE_INCOMINGMSG_HPP_ */
