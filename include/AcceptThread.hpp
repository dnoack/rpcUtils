/*
 * AcceptThread.hpp
 *
 *  Created on: 30.05.2015
 *      Author: Dave
 */

#ifndef ACCEPTTHREAD_HPP_
#define ACCEPTTHREAD_HPP_



#include <pthread.h>
#include <stdlib.h>
#include <ctime>

#define TIMEOUT 2



class AcceptThread
{

	public:

		/**Constructor*/
		AcceptThread()
		{
			_accepter = 0;
			accept_thread_active = false;
	    }


		/**Destructor*/
		virtual ~AcceptThread()
		{
			accept_thread_active = false;
		}


		pthread_t StartAcceptThread()
		{
		  pthread_create(&_accepter, NULL, thread_acceptEntryFunc, this);
		  return _accepter;
		}


		/** Will not return until the internal listener thread has exited. */
		void WaitForAcceptThreadToExit()
		{
		   (void) pthread_join(_accepter, NULL);
		}


		pthread_t getAccepter(){return _accepter;}


		int wait_for_accepter_up()
		{
		   time_t startTime = time(NULL);
		   while(time(NULL) - startTime < TIMEOUT)
		   {
			   if(_accepter != 0)
				   return 0;
		   }
		   return -1;
		}


		protected:

			/** This method has to be responsible for the connection setup and execution tasks like prngd computation.*/
			virtual void thread_accept() = 0;
			bool accept_thread_active;


		private:


			/** Creates a new listener thread and starts executing thread_listen within it.*/
			static void* thread_acceptEntryFunc(void* This)
			{
			   ((AcceptThread*)This)->thread_accept();
			   return NULL;
			}


			/** ID of the internal listener thread.*/
			pthread_t _accepter;

};


#endif /* ACCEPTTHREAD_HPP_ */
