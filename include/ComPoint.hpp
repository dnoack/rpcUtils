/*
 * ComPoint.hpp
 *
 *  Created on: 30.05.2015
 *      Author: Dave
 */

#ifndef COMPOINT_HPP_
#define COMPOINT_HPP_

#define JSON_ERROR_RESPONSE_INCORECCT_MSG "{\"jsonrpc\": \"2.0\", \"error\": {\"code\":  -1, \"message\": \"Received message with invalid packet encoding.\"}, \"id\": 0}"

#include <list>
#include <unistd.h> //close
#include <sys/socket.h> //send, recv
#include <sys/select.h>
#include <pthread.h>
#include "signal.h"


#include "RPCMsg.hpp"
#include "WorkerThreads.hpp"
#include "LogUnit.hpp"
#include "ProcessInterface.hpp"
#include "Error.hpp"

#define BUFFER_SIZE 2048 //in byte
#define TAG_LENGTH  1
#define LENGTH_LENGTH  4
#define HEADER_SIZE TAG_LENGTH + LENGTH_LENGTH //in byte
#define RECV_TIMEOUT 3//in seconds

class ProcessInterface;

class ComPoint :  public WorkerThreads, public LogUnit{

	public:

		ComPoint(int socket, ProcessInterface* pInterface, int uniqueID, bool viceVersaRegister = true, bool startInstant = true);

		virtual ~ComPoint();

		void configureLogInfo(LogInformation* in, LogInformation* out, LogInformation* info );

		void startWorking(){StartWorkerThread();}

		ProcessInterface* getProcessInterface(){return pInterface;}

		int transmit(const char* data, int size);

		int transmit(RPCMsg* rpcMsg);

		bool isDeletable(){return deletable;}

		void push_frontReceiveQueue(RPCMsg* data)
		{
			pthread_mutex_lock(&rQmutex);
				receiveQueue.push_front(data);
			pthread_mutex_unlock(&rQmutex);
		}

	protected:

		int uniqueID;

		//receivequeue
		list<RPCMsg*> receiveQueue;
		pthread_mutex_t rQmutex;

		char receiveBuffer[BUFFER_SIZE];
		int recvSize;
		char* msgBuffer;
		int msgBufferSize;
		int oldMsgBufferSize;
		char* tempBuffer;
		int messageSize;

		char* headerIn;
		char* headerOut;

		fd_set rfds;
		struct timeval timeout;

		/*! The socket of this worker, which is used to send and receive data.*/
		int currentSocket;

		//signal variables
		sigset_t sigmask;
		sigset_t origmask;
		int currentSig;


		bool deletable;

		/*! Contains Log-information for logging incomming data.*/
		LogInformation logInfoIn;
		/*! Contains Log-information for logging outgoing data.*/
		LogInformation logInfoOut;

		virtual void thread_listen();
		virtual void thread_work();

		void waitForFurtherData();

		void createHeader(char* header,  int value);
		unsigned int readHeader(char* buffer);

	private:

		ProcessInterface* pInterface;


		void deleteReceiveQueue();
		void popReceiveQueue();
		void popReceiveQueueWithoutDelete();
		void push_backReceiveQueue(RPCMsg* rpcMsg);
		int getReceiveQueueSize();

		void configSignals();
		void deleteBuffer(char* buffer);

};

#endif /* COMPOINT_HPP_ */
