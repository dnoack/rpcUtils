#ifndef INCLUDE_ProcessInterfaceB_HPP_
#define INCLUDE_ProcessInterfaceB_HPP_


#include <pthread.h>
#include "RPCMsg.hpp"
#include "Error.hpp"
#include "ProcessInterface.hpp"


/**
 * \interface ProcessInterfaceB
 * Adds the definition of the pure virtual function isSubResponse and the function
 * setComPoint(ComPoint*) to the baseclass ProcessInterface. These are necessary features
 * if we want to send subRequests (over ComPoint) and receive subResponses (ComPointB using
 * isSubResponse method within listener).
 */
class ProcessInterfaceB : public ProcessInterface{


	public:
		/** Base-Constructor.*/
		ProcessInterfaceB()
		{
			this->comPoint = NULL;
		};

		/** Base-Desctructor.*/
		virtual ~ProcessInterfaceB(){};

		/** Setting the comPoint, so we can use it's transmit methods to send subRequests.*/
		void setComPoint(ComPoint* comPoint){this->comPoint = comPoint;}

		/** The implementation of this should analyse a incoming message if it is a response message
		 * and if the json rpc id is the corresponding one of the send subRequest.
		 * \note Has to be implemented by the class which inherits from ProcessInterfaceB.
		 */
		virtual bool isSubResponse(RPCMsg* rpcMsg) = 0;

	protected:
		ComPoint* comPoint;

};


#endif /* INCLUDE_ProcessInterfaceB_HPP_ */
