/*
 * ProcessInterface.hpp
 *
 *  Created on: 30.05.2015
 *      Author: Dave
 */

#ifndef PROCESSINTERFACE_HPP_
#define PROCESSINTERFACE_HPP_

#include <pthread.h>
#include "ComPoint.hpp"
#include "Error.hpp"

class ComPoint;

class ProcessInterface{


	public:
		ProcessInterface()
		{
			pthread_mutex_init(&processMutex, NULL);
			pthread_mutex_init(&busyMutex, NULL);
			comPoint = NULL;
			busy = false;
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


		void setComPoint(ComPoint* comPoint)
		{
			this->comPoint = comPoint;
		}


		bool isBusy()
		{
			bool result = false;
			pthread_mutex_lock(&busyMutex);
			result = this->busy;
			pthread_mutex_unlock(&busyMutex);
			return result;
		}


	protected:

		ComPoint* comPoint;

		void setBusy(bool value)
		{
			pthread_mutex_lock(&busyMutex);
			this->busy = value;
			pthread_mutex_unlock(&busyMutex);
		}



	private:
		pthread_mutex_t processMutex;
		pthread_mutex_t busyMutex;
		bool busy;

		virtual void process(RPCMsg* rpcMsg) = 0;

};


#endif /* PROCESSINTERFACE_HPP_ */
