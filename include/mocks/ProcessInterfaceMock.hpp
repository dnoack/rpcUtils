/*
 * ProcessInterfaceMock.hpp
 *
 *  Created on: 03.06.2015
 *      Author: dave2
 */

#ifndef INCLUDE_MOCKS_PROCESSINTERFACEMOCK_HPP_
#define INCLUDE_MOCKS_PROCESSINTERFACEMOCK_HPP_

#include "ProcessInterface.hpp"


class ProcessInterfaceMock : public ProcessInterface
{
	public:
		ProcessInterfaceMock()
		{
			msgReceived = false;
			input = NULL;
			output = NULL;
		};

		virtual ~ProcessInterfaceMock(){};


		void reset()
		{
			msgReceived = false;
			delete output;
		}

		string* getReceivedData(){return output->getContent();}

	private:

		OutgoingMsg* output;
		IncomingMsg* input;
		bool msgReceived;

		OutgoingMsg* process(IncomingMsg* input)
		{
		    output = new OutgoingMsg(input->getOrigin(), input->getContent());
		    delete input;
			msgReceived = true;
			return NULL;
		}
};



#endif /* INCLUDE_MOCKS_PROCESSINTERFACEMOCK_HPP_ */
