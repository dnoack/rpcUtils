/*
 * ProcessInterfaceB.hpp
 *
 *  Created on: 08.06.2015
 *      Author: dave2
 */

#ifndef INCLUDE_ProcessInterfaceB_HPP_
#define INCLUDE_ProcessInterfaceB_HPP_



#include <pthread.h>
#include "RPCMsg.hpp"
#include "Error.hpp"
#include "ProcessInterface.hpp"
#include "WorkerInterface.hpp"


class ProcessInterfaceB : public ProcessInterface{


	public:
		ProcessInterfaceB()
		{
			subMsg = NULL;
		};

		virtual ~ProcessInterfaceB(){};


		virtual void isSubResponse(RPCMsg* rpcMsg) = 0;


		void setSubMsg(RPCMsg* subMsg){this->subMsg = subMsg;}


	protected:

		RPCMsg* subMsg;

};


#endif /* INCLUDE_ProcessInterfaceB_HPP_ */
