/*
 * ComPointB.hpp
 *
 *  Created on: 08.06.2015
 *      Author: dave2
 */

#ifndef INCLUDE_COMPOINTB_HPP_
#define INCLUDE_COMPOINTB_HPP_


#include "ProcessInterfaceB.hpp"
#include "ComPoint.hpp"

class ComPointB : public ComPoint{

	public:
		ComPointB(int socket, ProcessInterfaceB* pInterface, int uniqueID) :
			ComPoint(socket, (ProcessInterface*)pInterface, uniqueID, false)
		{
			this->pInterface = pInterface;
			StartWorkerThread();

			if(wait_for_listener_up() != 0)
				throw Error("Creation of Listener/worker threads failed.");
			else
				dlog(logInfo, "Created ComPoint with uniqueID %d and socket number: %d.", uniqueID, socket);
		};

		virtual ~ComPointB(){};

		void startWorking(){StartWorkerThread();}

		ProcessInterfaceB* getProcessInterface(){return pInterface;}


	private:

		virtual void thread_listen();


	private:

		ProcessInterfaceB* pInterface;
};



#endif /* INCLUDE_COMPOINTB_HPP_ */
