#ifndef INCLUDE_COMPOINTB_HPP_
#define INCLUDE_COMPOINTB_HPP_


#include "ProcessInterfaceB.hpp"
#include "ComPoint.hpp"


/**
 * \class ComPointB
 * Most methods are just like the ones from ComPoint. The method thread_listen has additional features for receiving subResponses
 * and because of that it uses ProcessInterfaceB.
 */
class ComPointB : public ComPoint{

	public:

		/**
		 * Like the Constructor of ComPoint but it takes a instance of ProccesInterfaceB as pInterface. The constructor of
		 * the base class ComPoint is called with the parameter startInstant = false. This is because we want to start the threads
		 * after pInterface of ComPointB is set, not before!
		 * \param socket The socket which should be used for receive and transmit data.
		 * \param pInterface A instance of a class which inherits from ProcessInterfaceB and implements the processMsg() method.
		 * \param uniqueID Unique identifier which should be 0 for RSD front and greater 0 for a plugin.
		 * \throws Error If the listener is not started during a defined time (defined by WorkerThread.hpp).
		 */
		ComPointB(int socket, ProcessInterfaceB* pInterface, int uniqueID, bool startInstant=false) :
			ComPoint(socket, (ProcessInterface*)pInterface, uniqueID, startInstant)
		{
			this->pInterface = pInterface;
			pInterface->setComPoint(this); //important for the ability of sending subresponses
		};

		/** Base-Destructor */
		virtual ~ComPointB(){};

		/** \return The corresponding ProcessInterfaceB. */
		ProcessInterfaceB* getProcessInterface(){return pInterface;}


	private:

		/**
		 * Additional to the base functions of ComPoint, this thread_listen function will
		 * use additional features of ProccesInterfaceB to check if a incoming message is a subResponse
		 * message. If a subResponse is received the signal SIGUSR2 will be send to the worker, which just
		 * waits for this signal within the processMsg() function. Else the message will be queued like any other
		 * message.
		 */
		virtual void thread_listen();


	private:

		/*! Correspindg instance of a class which inherits from ProcessInterfaceB.*/
		ProcessInterfaceB* pInterface;
};


#endif /* INCLUDE_COMPOINTB_HPP_ */
