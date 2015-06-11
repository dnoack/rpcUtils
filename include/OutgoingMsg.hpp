/*
 * OutgoingMsg.hpp
 *
 *  Created on: 10.06.2015
 *      Author: dave2
 */

#ifndef INCLUDE_OUTGOINGMSG_HPP_
#define INCLUDE_OUTGOINGMSG_HPP_

#include "stdio.h"
#include "RPCMsg.hpp"

using namespace std;


class OutgoingMsg : public RPCMsg{

	public:
		OutgoingMsg(ComPoint* destination, string* content )
		{
			//outgoingmsg content will be copied
			this->content = new string(*content);
			this->comPoint = destination;
		};

		OutgoingMsg(ComPoint* destination, const char* content )
		{
			this->content = new string(content);
			this->comPoint = destination;
		};

		virtual ~OutgoingMsg(){}

		string* getContent(){return this->content;}
		ComPoint* getDestination(){return this->comPoint;}

};



#endif /* INCLUDE_OUTGOINGMSG_HPP_ */
