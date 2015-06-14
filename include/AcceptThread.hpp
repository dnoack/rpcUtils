#ifndef ACCEPTTHREAD_HPP_
#define ACCEPTTHREAD_HPP_


#include <pthread.h>
#include <stdlib.h>
#include <ctime>

#define TIMEOUT 2


/**
 * \class AcceptThread
 * Another class can use AcceptThread as baseclass for creating a separate thread which has access to the members of
 * this class. The separate thread should be used for accepting new incomming connection like tcp or ipc. Therefore
 * AcceptThread defines the pure virtual function thread_accept, which has to be implemented by the class which inherits
 * from AcceptThread. Everything within thread_accept will run in the separate new thread. For starting the thread one
 * can use StartAcceptThread(). Stopping the thread from working is dedicated to the programmer.If you have a cancellation
 * point within your thread_accept method, you can use pthread_cancel(getAccepter()) in combination with WaitForAcceptThreadToExit()
 * to close the thread.
 */
class AcceptThread
{

	public:

		/**Base-Constructor*/
		AcceptThread()
		{
			_accepter = 0;
			accept_thread_active = false;
	    }


		/**Base-Destructor*/
		virtual ~AcceptThread()
		{
			accept_thread_active = false;
		}


		/**
		 *Starts a new thread and executes code within thread_accept().
		 */
		pthread_t StartAcceptThread()
		{
		  //as argument we use the this-pointer, which gives us the possibility to access class elements.
		  pthread_create(&_accepter, NULL, thread_acceptEntryFunc, this);
		  return _accepter;
		}


		/** Will not return until the separate thread has exited. */
		void WaitForAcceptThreadToExit()
		{
		   (void) pthread_join(_accepter, NULL);
		   _accepter = 0;
		}

		/** \return The thread id of the separate thread. This is 0 if not thread is running.*/
		pthread_t getAccepter(){return _accepter;}


		/**
		 * Checks the internal thread id of the separate thread for not being zero.
		 * The id will be checked for the time of TIMEOUT, defined in AcceptThread.hpp.
		 * \return If the thread id becomes unequal 0 it will return 0, if the time of timeout exceeded it return -1.
		 */
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

			/**Your code which should run in a separate thread has to be implemented in this function at your class which inherits AcceptThread.*/
			virtual void thread_accept() = 0;
			/** Flag which can be used to start/cancel a loop within the thread.*/
			bool accept_thread_active;


		private:

			/**Wrapper function to start the thread_accept function of the child class.*/
			static void* thread_acceptEntryFunc(void* This)
			{
			   //because of this cast we can use a non static member function instead of a static function for a thread.
			   ((AcceptThread*)This)->thread_accept();
			   return NULL;
			}

			/** ID of the internal thread.*/
			pthread_t _accepter;
};


#endif /* ACCEPTTHREAD_HPP_ */
