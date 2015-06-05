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
#include "Error.hpp"
#include "WorkerInterface.hpp"


class ProcessInterface{


	public:
		ProcessInterface()
		{
			pthread_mutex_init(&processMutex, NULL);
			pthread_mutex_init(&busyMutex, NULL);
			workerInterface = NULL;
			busy = false;
			subMsg = NULL;
		};


		virtual ~ProcessInterface()
		{
			pthread_mutex_destroy(&processMutex);
			pthread_mutex_destroy(&busyMutex);
		};

		void processMsg(RPCMsg* rpcMsg)
		{
			pthread_mutex_lock(&processMutex);
			try
			{
				process(rpcMsg);
			}
			catch(Error &e)
			{
				pthread_mutex_unlock(&processMutex);
				//throw to ComPoint
				throw;
			}
			catch(...)
			{
				pthread_mutex_unlock(&processMutex);
				//throw to ComPoint
				throw;
			}
			//everything is fine
			pthread_mutex_unlock(&processMutex);
		}


		void setWorkerInterface(WorkerInterface<RPCMsg>* workerInterface)
		{
			this->workerInterface = workerInterface;
		}

		void setSubMsg(RPCMsg* subMsg){this->subMsg = subMsg;}


		bool isBusy()
		{
			bool result = false;
			pthread_mutex_lock(&busyMutex);
			result = busy;
			pthread_mutex_unlock(&busyMutex);
			return result;
		}


	protected:

		WorkerInterface<RPCMsg>* workerInterface;
		RPCMsg* subMsg;

		void setBusy(bool value)
		{
			pthread_mutex_lock(&busyMutex);
			busy = value;
			pthread_mutex_unlock(&busyMutex);
		}



	private:
		pthread_mutex_t processMutex;
		pthread_mutex_t busyMutex;
		bool busy;

		virtual void process(RPCMsg* rpcMsg) = 0;

};


#endif /* PROCESSINTERFACE_HPP_ */