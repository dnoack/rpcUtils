/*
 * ComPoint.hpp
 *
 *  Created on: 30.05.2015
 *      Author: Dave
 */

#ifndef COMPOINT_HPP_
#define COMPOINT_HPP_

#define JSON_ERROR_RESPONSE_INCORECCT_MSG "{\"jsonrpc\": \"2.0\", \"error\": {\"code\":  -1, \"message\": \"Received message with invalid packet encoding.\"}, \"id\": 0}"

#include <list> //receiveQueue
#include <unistd.h> //close
#include <sys/socket.h> //send, recv
#include <sys/select.h> //waitForFurtherData
#include <pthread.h>
#include "signal.h"


#include "RPCMsg.hpp"
#include "WorkerThreads.hpp"
#include "LogUnit.hpp"
#include "ProcessInterface.hpp"
#include "Error.hpp"

#define BUFFER_SIZE 2048 //Size of ReceiveBuffer in size
#define TAG_LENGTH  1 //Size of TAG-field in byte
#define LENGTH_LENGTH  4 //Size of LENGTH-field in byte
#define HEADER_SIZE TAG_LENGTH + LENGTH_LENGTH //Size of complete HEADer in byte
#define RECV_TIMEOUT 3 //Timeout for Receiving a whole encoded msg in seconds

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


		//sometimes a processInterface has to analyse data and then put it in queue, so we need it public
		void push_frontReceiveQueue(RPCMsg* data)
		{
			pthread_mutex_lock(&rQmutex);
				receiveQueue.push_front(data);
			pthread_mutex_unlock(&rQmutex);
		}

	protected:

		/*! Unique id of this Compoint. If this Compoint is connected to a Plugin, the plugin id is the unique id*/
		int uniqueID;

		/*!All incomming messages except subResponses are pushed to this queue.*/
		list<RPCMsg*> receiveQueue;
		/*! Mutex to protect the receiveQueue, because thread_listen can push and thread_work can pop.*/
		pthread_mutex_t rQmutex;

		/*! ReceiveBuffer of thread_listen, this buffer has to be big enough.*/
		char receiveBuffer[BUFFER_SIZE];

		char* msgBuffer;
		int msgBufferSize;

		char* tempBuffer;

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
