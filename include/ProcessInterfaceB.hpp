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


class ProcessInterfaceB : public ProcessInterface{


	public:
		ProcessInterfaceB(){};

		virtual ~ProcessInterfaceB(){};

		void setComPoint(ComPoint* comPoint){this->comPoint = comPoint;}

		virtual bool isSubResponse(RPCMsg* rpcMsg) = 0;

};


#endif /* INCLUDE_ProcessInterfaceB_HPP_ */
