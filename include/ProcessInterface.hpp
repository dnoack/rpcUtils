/*
 * ProcessInterface.hpp
 *
 *  Created on: 30.05.2015
 *      Author: Dave
 */

#ifndef PROCESSINTERFACE_HPP_
#define PROCESSINTERFACE_HPP_

#include <pthread.h>
#include "RPCMsg.hpp"
#include "WorkerInterface.hpp"

class ProcessInterface{


	public:
		ProcessInterface()
		{
			pthread_mutex_init(&processMutex, NULL);
			workerInterface = NULL;
		};


		virtual ~ProcessInterface()
		{
			pthread_mutex_destroy(&processMutex);
		};

		void processMsg(RPCMsg* rpcMsg)
		{
			pthread_mutex_lock(&processMutex);
			process(rpcMsg);
			pthread_mutex_unlock(&processMutex);
		}

		void setWorkerInterface(WorkerInterface<RPCMsg>* workerInterface)
				{
					this->workerInterface = workerInterface;
				}

	protected:
		WorkerInterface<RPCMsg>* workerInterface;




	private:
		pthread_mutex_t processMutex;

		virtual void process(RPCMsg* rpcMsg) = 0;

};




#endif /* PROCESSINTERFACE_HPP_ */
