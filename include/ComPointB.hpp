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
		ComPointB(int socket, ProcessInterfaceB* pInterface, int uniqueID, bool viceVersaRegister = true) :
			ComPoint(socket, (ProcessInterface*)pInterface, uniqueID, viceVersaRegister, false)
		{
			this->pInterface = pInterface;
			StartWorkerThread();

			if(wait_for_listener_up() != 0)
				throw Error("Creation of Listener/worker threads failed.");
			else
				dlog(logInfo, "Created ComPoint.");
				//TODO: add some specific information about this compoint
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
