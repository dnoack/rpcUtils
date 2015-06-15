#ifndef INCLUDE_OUTGOINGMSG_HPP_
#define INCLUDE_OUTGOINGMSG_HPP_

#include "stdio.h"
#include "RPCMsg.hpp"

using namespace std;


/**
 * \class OutgoingMsg
 * This class inherits from RPCMsg and contains no additional information. But for
 * better usage it specifies the internal instance of ComPoint as destination of the JSON RPC
 * message. As the name says, messages which has to be send should be saved and used as OutgoingMsg.
 */
class OutgoingMsg : public RPCMsg{

	public:


		/**
		 * Constructor for setting the corresponding ComPoint and the message itself.
		 * \param destination The ComPoint which should send this message.
		 * \param content The JSON RPC message as string.
		 * \note Parameter content will NOT be copied.
		 */
		OutgoingMsg(ComPoint* destination, string* content )
		{
			//outgoingmsg content will be copied
			this->content = new string(*content);
			this->comPoint = destination;
		};

		/**
		 *
		 * Constructor for setting the corresponding ComPoint and the message itself.
		 * \param destination The ComPoint which should send this message.
		 * \param content The JSON RPC message as const char array.
		 * \note Parameter content will be copied.
		 */
		OutgoingMsg(ComPoint* destination, const char* content )
		{
			this->content = new string(content);
			this->comPoint = destination;
		};

		/** Base-Destructor.*/
		virtual ~OutgoingMsg(){}

		/** \return The content (JSON RPC message) of this container.*/
		string* getContent(){return this->content;}
		/** \return The target ComPoint which should send this message.*/
		ComPoint* getDestination(){return this->comPoint;}

};



#endif /* INCLUDE_OUTGOINGMSG_HPP_ */
