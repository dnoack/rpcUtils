/*
 * OutgoingMsg.hpp
 *
 *  Created on: 10.06.2015
 *      Author: dave2
 */

#ifndef INCLUDE_OUTGOINGMSG_HPP_
#define INCLUDE_OUTGOINGMSG_HPP_

#include "stdio.h"
#include "ComPoint.hpp"

using namespace std;

class ComPoint;

class OutgoingMsg{

	public:
		OutgoingMsg(string* content, int uniqueId, ComPoint* comPoint = NULL)
		{
			this->content = new string(*content);
			this->uniqueId = uniqueId;
			this->comPoint = comPoint;
		}

		OutgoingMsg(const char* content, int uniqueId, ComPoint* comPoint = NULL)
		{
			this->content = new string(content);
			this->uniqueId = uniqueId;
			this->comPoint = comPoint;
		}

		virtual ~OutgoingMsg()
		{
			delete content;
		}

		string* getContent(){return this->content;}
		int getUniqueId(){return this->uniqueId;}
		ComPoint* getComPoint(){return this->comPoint;}

	private:
		string* content;
		int uniqueId;
		ComPoint* comPoint;
};



#endif /* INCLUDE_OUTGOINGMSG_HPP_ */
