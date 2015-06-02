
#ifndef WORKERINTERFACE_HPP_
#define WORKERINTERFACE_HPP_

#include <list>
#include <string>
#include <pthread.h>
#include <bitset>
#include <cstring>
#include <sys/select.h>
#include <sys/socket.h>
#include "signal.h"

#include "LogUnit.hpp"


#define BUFFER_SIZE 2048 //in byte
#define TAG_LENGTH  1
#define LENGTH_LENGTH  4
#define HEADER_SIZE TAG_LENGTH + LENGTH_LENGTH //in byte
#define RECV_TIMEOUT 3//in seconds


using namespace std;


template <class TMsg>
class WorkerInterface{

	public:

		WorkerInterface()
		{
			pthread_mutex_init(&rQmutex, NULL);

			currentSig = 0;
			currentSocket = 0;
			recvSize = 0;

			msgBuffer = NULL;
			msgBufferSize = 0;
			oldMsgBufferSize = 0;
			tempBuffer = NULL;
			messageSize = 0;

			timeout.tv_sec = 3;
			timeout.tv_usec = 0;


			deletable = false;
			logInfoIn.logLevel = 1;
			logInfoIn.logName = "";
			logInfoOut.logLevel = 2;
			logInfoOut.logName = "";

			headerIn = new char[HEADER_SIZE];
			headerOut = new char[HEADER_SIZE];


			memset(receiveBuffer, '\0', BUFFER_SIZE);
			memset(headerIn, '\0', HEADER_SIZE);
			memset(headerOut, '\0', HEADER_SIZE);
		};


		virtual ~WorkerInterface()
		{
			delete[] headerIn;
			delete[] headerOut;
			pthread_mutex_destroy(&rQmutex);
		};

		bool isDeletable(){return deletable;}

		void push_frontReceiveQueue(TMsg* data)
		{
			pthread_mutex_lock(&rQmutex);
				receiveQueue.push_front(data);
			pthread_mutex_unlock(&rQmutex);
		}

		virtual int transmit(const char* data, int size) = 0;
		virtual int transmit(TMsg* msg) = 0;



	protected:

		//receivequeue
		list<TMsg*> receiveQueue;
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


		void createHeader(char* header,  int value)
		{
			memset(header, '\0', HEADER_SIZE);
			header[0] = (char)93; // we are wizards
			if(value > 0)
			{
				for(int i = HEADER_SIZE-1 ; i > 0 ; i--)
				{
					header[i] = (char)value;
					value = value >> 8;
				}
			}
		}


		unsigned int readHeader(char* buffer)
		{
			unsigned int value = 0;
			unsigned int temp = 0;
			int count = HEADER_SIZE-1;

			if(buffer[0] != (int)93)
				return -1;
			else
			{
				//begin right side of array, which is lowest byte
				for(int i = count; i > 0 ; i--)
				{
					temp = ((unsigned char)buffer[i]);
					//first byte shift 0 left, second 8 bit, third 16 bit....
					temp = temp << (count-i)*8;
					//add temp value to final value
					value |= temp;
				}
			}

			return value;
		}





		void waitForFurtherData()
		{
			//wait for more with select and timeout
			int retval = select(currentSocket+1, &rfds, NULL, NULL, &timeout);
			if(retval > 0)
			{
				recvSize = recv(currentSocket , receiveBuffer, BUFFER_SIZE, 0);

				//save the size of the currentMsgbuffer, allocate a bigger buffer for old+ new data
				oldMsgBufferSize = msgBufferSize;
				msgBufferSize = msgBufferSize + recvSize;
				tempBuffer = new char[msgBufferSize];
				memset(tempBuffer, '\0', msgBufferSize);

				//copy old data, then copy new data to new buffer, delete old one
				memcpy(tempBuffer, msgBuffer, oldMsgBufferSize);
				memcpy(&tempBuffer[oldMsgBufferSize], receiveBuffer, recvSize);
				delete[] msgBuffer;
				msgBuffer = tempBuffer;
			}
			else if(retval == 0)
			{
				delete[] msgBuffer;
				msgBufferSize = 0;
			}
			else
			{
				//errno prüfen
				//error msg senden
			}
		}



		void deleteReceiveQueue()
		{
			typename list<TMsg*>::iterator i;
			pthread_mutex_lock(&rQmutex);
			if(receiveQueue.size() > 0)
			{
				i = receiveQueue.begin();
				while(i != receiveQueue.end())
				{
					delete *i;
					i = receiveQueue.erase(i);
				}
			}

			pthread_mutex_unlock(&rQmutex);
		}

		void popReceiveQueue()
		{
			TMsg* lastElement = NULL;
			pthread_mutex_lock(&rQmutex);
			lastElement = receiveQueue.back();
			receiveQueue.pop_back();
			delete lastElement;
			pthread_mutex_unlock(&rQmutex);
		}

		void popReceiveQueueWithoutDelete()
		{
			pthread_mutex_lock(&rQmutex);
				receiveQueue.pop_back();
			pthread_mutex_unlock(&rQmutex);
		}




		void push_backReceiveQueue(TMsg* data)
		{
			pthread_mutex_lock(&rQmutex);
				receiveQueue.push_back(data);
			pthread_mutex_unlock(&rQmutex);
		}


		int getReceiveQueueSize()
		{
			int result = 0;
			pthread_mutex_lock(&rQmutex);
				result = receiveQueue.size();
			pthread_mutex_unlock(&rQmutex);

			return result;
		}


		void configSignals()
		{
			sigemptyset(&origmask);
			sigemptyset(&sigmask);
			sigaddset(&sigmask, SIGUSR1);
			sigaddset(&sigmask, SIGUSR2);
			pthread_sigmask(SIG_BLOCK, &sigmask, &origmask);
		}

};

#endif /* WORKERINTERFACE_HPP_ */
