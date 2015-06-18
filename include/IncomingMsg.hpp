#ifndef INCLUDE_INCOMINGMSG_HPP_
#define INCLUDE_INCOMINGMSG_HPP_

#include "stdio.h"
#include "RPCMsg.hpp"

using namespace std;

class ComPoint;


/**
 * \class IncomingMsg
 * This class inherits from RPCMsg and contains no additional information. But for
 * better usage it specifies the internal instance of ComPoint as origin of the JSON RPC
 * message. As the name says, all received messages should be saved and used as IncommingMsg.
 * \see OutgoingMsg
 */
class IncomingMsg : public RPCMsg{

	public:

		/**
		 * Constructor for setting the corresponding ComPoint and the message itself.
		 * \param origin The ComPoint where the message was received with.
		 * \param content The JSON RPC message as string.
		 * \note Parameter content will NOT be copied.
		 */
		IncomingMsg(ComPoint* origin, string* content )
		{
			//incommingMsg content will NOT be copied
			this->content = content;;
			this->comPoint = origin;
		}

		/**
		 * Constructor for setting the corresponding ComPoint and the message itself.
		 * \param origin The ComPoint where the message was received with.
		 * \param content The JSON RPC message as const char array.
		 * \note Parameter content will be copied.
		 */
		IncomingMsg(ComPoint* origin, const char* content )
		{
			this->content = new string(content);
			this->comPoint = origin;
		}

		/** Base Destructor.*/
		~IncomingMsg(){}

		/** \return The content (JSON RPC message) of this container.*/
		string* getContent(){return this->content;}
		/** \return The ComPoint which received the JSON RPC message.*/
		ComPoint* getOrigin(){return this->comPoint;}
};


#endif /* INCLUDE_INCOMINGMSG_HPP_ */
