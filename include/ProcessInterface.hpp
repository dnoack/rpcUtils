#ifndef PROCESSINTERFACE_HPP_
#define PROCESSINTERFACE_HPP_

#include <IncomingMsg.hpp>
#include <pthread.h>
#include "OutgoingMsg.hpp"
#include "Error.hpp"

/**
 * \interface ProcessInterface
 * ProccessInterface is a interface to connect classes with functions which should be remotely executed through
 * a ComPoint. Every class which inherits from ProcessInterface has to implemented the function
 * process(IncommingMsg*), where the the received message can be parsed and the corresponding RPC executed.
 * The public method processMsg(IncommingMsg*) is the interface for the overlying ComPoint.
 */
class ProcessInterface{


	public:

		/** Base-Constructor.*/
		ProcessInterface()
		{
			pthread_mutex_init(&processMutex, NULL);
			pthread_mutex_init(&busyMutex, NULL);
			busy = false;
			//comPoint = NULL;
		};

		/** Base-Desctructor.*/
		virtual ~ProcessInterface()
		{
			pthread_mutex_destroy(&processMutex);
			pthread_mutex_destroy(&busyMutex);
		};

		/*
		 * Wrapper function for process(). It just calls process(),
		 * catches unexpected exception and rethrows to the overlying ComPoint.
		 * \note Mutex processMutex is used to protect the call of process().
		 */
		OutgoingMsg* processMsg(IncomingMsg* input)
		{
			OutgoingMsg* output = NULL;
			pthread_mutex_lock(&processMutex);
			try
			{
				output =  process(input);
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
			return output;
		}

		/*
		 * \return True if there is currently a message processing, else false.
		 * \note Uses mutex busyMutex.
		 */
		bool isBusy()
		{
			bool result = false;
			pthread_mutex_lock(&busyMutex);
			result = this->busy;
			pthread_mutex_unlock(&busyMutex);
			return result;
		}


	protected:

		/** Corresponding comPoint. Will only be used if there is need
		 * for subRequests during the process of a message.*/
		//ComPoint* comPoint;

		/**
		 * Sets the busy flag of this Interface to busy or not busy.
		 * \param True for busy, false for not busy.
		 * \note Uses mutex busyMutex.
		 */
		void setBusy(bool value)
		{
			pthread_mutex_lock(&busyMutex);
			this->busy = value;
			pthread_mutex_unlock(&busyMutex);
		}


	private:
		/*! Mutex fo protecting the call of process().*/
		pthread_mutex_t processMutex;
		/*! Mutex to protect setting/getting busy flag status.*/
		pthread_mutex_t busyMutex;
		/*! Shows if a message is currently processing or not.*/
		bool busy;

		/*!
		 * Definition of process(), which has to process an instance of IncommingMsg and can
		 * return an instance of OutgoingMsg.
		 * \param input The message which should be processed.
		 * \return NULL if the input triggers asynchrounous features, else there will be a instance of OutgoingMsg with a response as return.
		 * \note Classes which inherits from processInterface have to implement this function.
		 */
		virtual OutgoingMsg* process(IncomingMsg* input) = 0;

};


#endif /* PROCESSINTERFACE_HPP_ */
