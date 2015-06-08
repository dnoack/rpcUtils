/*
 * ComPointB.hpp
 *
 *  Created on: 08.06.2015
 *      Author: dave2
 */

#ifndef INCLUDE_COMPOINTB_HPP_
#define INCLUDE_COMPOINTB_HPP_


#define JSON_ERROR_RESPONSE_INCORECCT_MSG "{\"jsonrpc\": \"2.0\", \"error\": {\"code\":  -1, \"message\": \"Received message with invalid packet encoding.\"}, \"id\": 0}"

#include <unistd.h> //close
#include <sys/socket.h> //send, recv
#include <pthread.h>

#include "WorkerInterface.hpp"
#include "RPCMsg.hpp"
#include "WorkerThreads.hpp"
#include "LogUnit.hpp"
#include "ProcessInterfaceB.hpp"
#include "Error.hpp"

class ComPointB : public WorkerInterface<RPCMsg>, public WorkerThreads, public LogUnit{

	public:
		ComPointB(int socket, ProcessInterfaceB* pInterface, int uniqueID, bool viceVersaRegister = true);

		virtual ~ComPointB();

		void configureLogInfo(LogInformation* in, LogInformation* out, LogInformation* info );

		ProcessInterface* getProcessInterface(){return pInterface;}

		int transmit(const char* data, int size);

		int transmit(RPCMsg* rpcMsg);

	private:

		int uniqueID;

		virtual void thread_listen();

		virtual void thread_work();

	private:

		ProcessInterfaceB* pInterface;
};



#endif /* INCLUDE_COMPOINTB_HPP_ */
