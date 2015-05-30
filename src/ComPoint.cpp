#include "ComPoint.hpp"


ComPoint::ComPoint(int socket, ProcessInterface* pInterface)
{
	this->currentSocket = socket;
	this->pInterface = pInterface;
	pInterface->setWorkerInterface(this);

	StartWorkerThread();

	if(wait_for_listener_up() != 0)
		throw Error("Creation of Listener/worker threads failed.");
	else
		dlog(logInfo, "Created ComPoint.");
		//TODO: add some specific information about this compoint
}


ComPoint::~ComPoint()
{
	pthread_cancel(getListener());
	pthread_cancel(getWorker());

	WaitForListenerThreadToExit();
	WaitForWorkerThreadToExit();

	delete pInterface;
}



void ComPoint::configureLogInfo(LogInformation* in, LogInformation* out, LogInformation* info)
{
	logInfoIn.logLevel = in->logLevel;
	logInfoIn.logName = in->logName;

	logInfoOut.logLevel = out->logLevel;
	logInfoOut.logName = out->logName;

	logInfo.logLevel = info->logLevel;
	logInfo.logName = info->logName;
}


void ComPoint::thread_work()
{
	worker_thread_active = true;
	RPCMsg* msg = NULL;

	configSignals();
	StartListenerThread();

	while(worker_thread_active)
	{
		sigwait(&sigmask, &currentSig);
		switch(currentSig)
		{
			case SIGUSR1:
				while(getReceiveQueueSize() > 0)
				{
					try
					{
						msg = receiveQueue.back();
						log(logInfoIn, msg->getContent());
						popReceiveQueueWithoutDelete();
						pInterface->processMsg(msg);
					}
					catch(...)
					{
						dyn_print("Unkown Exception.\n");
					}

				}
			break;

			default:
				dyn_print("ComPoint: Unkown signal \n");
				break;
		}
	}
	close(currentSocket);
}



void ComPoint::thread_listen()
{
	listen_thread_active = true;
	string* content = NULL;
	int retval = 0;
	pthread_t worker_thread = getWorker();


	FD_ZERO(&rfds);
	FD_SET(currentSocket, &rfds);


	while(listen_thread_active)
	{
		memset(receiveBuffer, '\0', BUFFER_SIZE);

		//hier nur hin wenn nichts mehr im buffer ist und auf keine teile einer nachricht gewartet wird.
		recvSize = recv(currentSocket , receiveBuffer, BUFFER_SIZE, 0);
		if(recvSize > 0)
		{
			//copy receiveBuffer to a clean msgBuffer
			msgBufferSize = recvSize;
			msgBuffer = new char[msgBufferSize];
			memset(msgBuffer, '\0', msgBufferSize);
			memcpy(msgBuffer, receiveBuffer, msgBufferSize);

			//As long as there is data in the msgBuffer
			while(msgBufferSize > 0)
			{
				//headersize = 1 byte tagfield + 4 byte lengthfield
				if(msgBufferSize > HEADER_SIZE)
				{
					messageSize = readHeader(msgBuffer);

					//header ok ???
					if(messageSize > -1)
					{
						//Is there at least one complete message in msgBuffer ?
						if(msgBufferSize >= messageSize+HEADER_SIZE)
						{
							//add first complete msg of msgbuffer to the receivequeue and signal the worker
							content = new string(&msgBuffer[HEADER_SIZE], messageSize);
							pushReceiveQueue(new RPCMsg(0, content));
							pthread_kill(worker_thread, SIGUSR1);

							//Is there more data ?
							if(msgBufferSize > messageSize+HEADER_SIZE)
							{
								//copy rest of data to a new clean msgBuffer
								msgBufferSize = msgBufferSize - (messageSize+HEADER_SIZE);
								tempBuffer = new char[msgBufferSize];
								memset(tempBuffer, '\0', msgBufferSize);
								memcpy(tempBuffer, &(msgBuffer[messageSize+HEADER_SIZE]), msgBufferSize);
								delete[] msgBuffer;
								msgBuffer = tempBuffer;
							}
							else
							{
								delete[] msgBuffer;
								msgBufferSize = 0;
							}
						}
						//message is not complete, wait for the rest
						else
						{
							waitForFurtherData();
						}
					}
					else
					{
						delete[] msgBuffer;
						msgBufferSize = 0;
						//TODO: send msg back, that something went wrong and the message was not correctly received
					}
				}
				//not even the header was complete, wait for the rest
				else
				{
					waitForFurtherData();
				}
			}
		}
		else
		{
			deletable = true;
			listen_thread_active = false;
		}
	}
}


int ComPoint::transmit(const char* data, int size)
{
	int sendCount = 0;
	createHeader(headerOut, size);
	log(logInfoOut, data);
	sendCount = send(currentSocket, headerOut, HEADER_SIZE, 0);
	sendCount += send(currentSocket, data, size, 0);
	return sendCount;
};


int ComPoint::transmit(RPCMsg* msg)
{
	int sendCount = 0;
	createHeader(headerOut, msg->getContent()->size());
	log(logInfoOut, msg->getContent());
	sendCount = send(currentSocket, headerOut, HEADER_SIZE, 0);
	sendCount += send(currentSocket, msg->getContent()->c_str(), msg->getContent()->size(), 0);
	return sendCount;
};



