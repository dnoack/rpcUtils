
#ifndef INCLUDE_WORKERTHREADS_HPP_
#define INCLUDE_WORKERTHREADS_HPP_


#include <pthread.h>
#include <stdlib.h>
#include <ctime>

#define TIMEOUT 2

class WorkerThreads;

struct argStruct
{
	WorkerThreads* th_id;
	int socket;
	char* buffer;
};



/**
* \class WorkerThreads
* \brief Implements Thread-handling for two threads within an object.
* WorkerThreads defines two methods thread_work() and thread_listen(prthread_t) which have to be implemented by
* classes who inherit from WorkerThreads. By implementing this class, the thread_work and thread_listen functions, it
* is possible to get two extra threads which work within the child class like they are NOT static. That means we can
* access all elements of this class like everything is in one thread.
* \note Of course this must be used with care, creating inconsistent data will be very easy.
*/
class WorkerThreads
{
public:

	/**Constructor*/
   WorkerThreads()
   {
	   _worker = 0;
	   _listener = 0;
	   worker_thread_active = false;
	   listen_thread_active = false;

   }
   /**Destructor*/
   virtual ~WorkerThreads()
   {
	   worker_thread_active = false;
	   listen_thread_active = false;
   }

   /**
   Starts the worker thread, which start executing the thread_work() function.
   \return True if the thread was successfully started, false if there was an error starting the thread.*/
   pthread_t StartWorkerThread()
   {
	  pthread_create(&_worker, NULL, thread_workEntryFunc, this);
	  return _worker;
   }

   /**
   Starts the listen thread, which start executing the thread_listen() function.
   \return True if the thread was successfully started, false if there was an error starting the thread.*/
   pthread_t StartListenerThread()
   {
	   pthread_create(&_listener, NULL, thread_listenerEntryFunc, this);
	   return _listener;
   }


   /** Will not return until the internal worker thread has exited. */
   void WaitForWorkerThreadToExit()
   {
      (void) pthread_join(_worker, NULL);
   }

   /** Will not return until the internal listener thread has exited. */
   void WaitForListenerThreadToExit()
   {
	   (void) pthread_join(_listener, NULL);
   }

   pthread_t getListener(){return _listener;}
   pthread_t getWorker(){return _worker;}


   int wait_for_listener_up()
   {
	   time_t startTime = time(NULL);
	   while(time(NULL) - startTime < TIMEOUT)
	   {
		   if(_listener != 0)
			   return 0;
	   }
	   return -1;
   }



protected:

	/** This method has to be responsible for the connection setup and execution tasks like prngd computation.*/
   virtual void thread_work() = 0;
   /**This method has to be responsible for listening to incomming data and signaling the worker.*/
   virtual void thread_listen()= 0;

   bool worker_thread_active;
   bool listen_thread_active;


private:

	/** Creates a new worker thread and starts executing thread_work within it.*/
   static void* thread_workEntryFunc(void * This)
   {
	   ((WorkerThreads*)This)->thread_work();
	   return NULL;
   }


   /** Creates a new listener thread and starts executing thread_listen within it.*/
   static void* thread_listenerEntryFunc(void* This)
   {
	   ((WorkerThreads*)This)->thread_listen();
	   return NULL;
   }


   /** Id of the internal worker thread.*/
   pthread_t _worker;
   /** ID of the internal listener thread.*/
   pthread_t _listener;

};


#endif /* INCLUDE_WORKERTHREADS_HPP_ */
