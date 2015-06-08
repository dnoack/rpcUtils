/*
 * ComPoint.hpp
 *
 *  Created on: 30.05.2015
 *      Author: Dave
 */

#ifndef COMPOINT_HPP_
#define COMPOINT_HPP_

#define JSON_ERROR_RESPONSE_INCORECCT_MSG "{\"jsonrpc\": \"2.0\", \"error\": {\"code\":  -1, \"message\": \"Received message with invalid packet encoding.\"}, \"id\": 0}"

#include <unistd.h> //close
#include <sys/socket.h> //send, recv
#include <pthread.h>

#include "WorkerInterface.hpp"
#include "RPCMsg.hpp"
#include "WorkerThreads.hpp"
#include "LogUnit.hpp"
#include "ProcessInterface.hpp"
#include "Error.hpp"


class ComPoint : public WorkerInterface<RPCMsg>, public WorkerThreads, public LogUnit{

	public:

		ComPoint(int socket, ProcessInterface* pInterface, int uniqueID, bool viceVersaRegister = true, bool startInstant = true);

		virtual ~ComPoint();

		void configureLogInfo(LogInformation* in, LogInformation* out, LogInformation* info );

		void startWorking(){StartWorkerThread();}

		ProcessInterface* getProcessInterface(){return pInterface;}

		int transmit(const char* data, int size);

		int transmit(RPCMsg* rpcMsg);

	protected:

		int uniqueID;

		virtual void thread_listen();
		virtual void thread_work();

	private:

		ProcessInterface* pInterface;

};

#endif /* COMPOINT_HPP_ */
