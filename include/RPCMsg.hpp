#ifndef RPCMSG_HPP_
#define RPCMSG_HPP_

#include <string>

/*! Unique id of a TCP ComPoint instance.*/
#define TCP_SIDE 0

using namespace std;

class ComPoint;

/**
 * \class RPCMsg
 * This is a container class for JSON RPC messages and some additional information. As additional
 * information it can contain a corresponding instance of ComPoint and after parsing the JSON RPC it can also contain
 * the json rpc id. This base class will only be used for saving instances of IncomingMsg and OutgoingMsg in a list.
 * Otherwise always IncomingMsg OR OutgoingMsg will be used.
 */
class RPCMsg{

	public:

		/** Base-Constructor, just for testing.*/
		RPCMsg()
		{
			this->comPoint = NULL;
			this->content = NULL;
			this->jsonRpcId = 0;
		};

		/**
		 * Constructor for setting the corresponding ComPoint and the message itself.
		 * \param comPoint The corresponding ComPoint instance.
		 * \param content The JSON RPC message as string.
		 * \note Parameter content will NOT be copied.
		 */
		RPCMsg(ComPoint* comPoint, string* content)
		{
			this->comPoint = comPoint;
			this->content = content;
			this->jsonRpcId = 0;
		};

		/**
		 * Constructor for setting the corresponding ComPoint and the message itself.
		 * \param comPoint The corresponding ComPoint instance.
		 * \param content The JSON RPC message as const char array.
		 * \note Parameter content will be copied.
		 */
		RPCMsg(ComPoint* comPoint, const char* content)
		{
			this->comPoint = comPoint;
			this->content = new string(content);
			this->jsonRpcId = 0;
		};

		/** Copy-Constructor. */
		RPCMsg(RPCMsg* rpcMsg)
		{
			this->comPoint = rpcMsg->getComPoint();
			this->content = new string(rpcMsg->getContent()->c_str(), rpcMsg->getContent()->size());
			this->jsonRpcId = rpcMsg->getJsonRpcId();
		};

		/** Base-Destructor.*/
		virtual ~RPCMsg()
		{
			delete content;
		};

		/** \return The internal JSON RPC.*/
		string* getContent(){return this->content;}
		/** \return The corresponding instance of ComPoint.*/
		ComPoint* getComPoint(){return this->comPoint;}
		/** \return True if the corresponding ComPoint is a TCP ComPoint, false otherwise.*/
		bool isOriginTcp();

		/** \return If the JSON RPC id is set, it returns its value, 0 otherwise.
		 * \note We can't know the JSON RPC id without parsing the internal message.*/
		int getJsonRpcId(){return this->jsonRpcId;}
		/** \param jsonRpcId Set the JSON RPC id for this container and it's message.*/
		void setJsonRpcId(int jsonRpcId){this->jsonRpcId = jsonRpcId;}


	protected:
		/*! This is the proper message, hopefully containing a valid json rpc message.*/
		string* content;
		/*! Corresponding instance of ComPoint for the message. It can be the origin or the destination of the message.*/
		ComPoint* comPoint;
		/*! After parsing content, this can contain the JSON RPC id of content.*/
		int jsonRpcId;
};


#endif /* RPCMSG_HPP_ */
