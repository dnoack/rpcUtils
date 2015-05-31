/*
 * ComPoint.hpp
 *
 *  Created on: 30.05.2015
 *      Author: Dave
 */

#ifndef COMPOINT_HPP_
#define COMPOINT_HPP_

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

		ComPoint(int socket, ProcessInterface* pInterface, int uniqueID);

		virtual ~ComPoint();

		void configureLogInfo(LogInformation* in, LogInformation* out, LogInformation* info );

		ProcessInterface* getProcessInterface(){return pInterface;}

		int transmit(const char* data, int size);

		int transmit(RPCMsg* rpcMsg);



	private:

		int uniqueID;

		ProcessInterface* pInterface;

		virtual void thread_listen();

		virtual void thread_work();


};

#endif /* COMPOINT_HPP_ */
