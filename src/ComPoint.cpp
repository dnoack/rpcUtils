#include "ComPoint.hpp"

ComPoint::ComPoint()
{
	this->currentSocket = 0;
	this->pInterface = NULL;
	this->uniqueID = 0;
	currentSig = 0;
	msgBuffer = NULL;
	msgBufferSize = 0;
	tempBuffer = NULL;
	headerOut = new char[HEADER_SIZE];
	deletable = false;
}


ComPoint::ComPoint(int socket, ProcessInterface* pInterface, int uniqueID, bool startInstant)
{
	this->currentSocket = socket;
	this->pInterface = pInterface;
	this->uniqueID = uniqueID;

	pthread_mutex_init(&rQmutex, NULL);

	currentSig = 0;

	msgBuffer = NULL;
	msgBufferSize = 0;
	tempBuffer = NULL;

	timeout.tv_sec = RECV_TIMEOUT;
	timeout.tv_usec = 0;

	deletable = false;
	logInfoIn.logLevel = 1;
	logInfoIn.logName = "";
	logInfoOut.logLevel = 2;
	logInfoOut.logName = "";

	headerOut = new char[HEADER_SIZE];

	FD_ZERO(&rfds);
	FD_SET(currentSocket, &rfds);

	memset(receiveBuffer, '\0', BUFFER_SIZE);
	memset(headerOut, '\0', HEADER_SIZE);


	if(startInstant)
	{
		StartWorkerThread();

		if(wait_for_listener_up() != 0)
			throw Error("Creation of Listener/worker threads failed.");
	}
}


ComPoint::~ComPoint()
{
	delete[] headerOut;

	pthread_cancel(getListener());
	pthread_cancel(getWorker());

	WaitForListenerThreadToExit();
	WaitForWorkerThreadToExit();

	close(currentSocket);

	deleteBuffer(msgBuffer);
	deleteBuffer(tempBuffer);

	pthread_mutex_destroy(&rQmutex);
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
	IncomingMsg* input = NULL;
	OutgoingMsg* output = NULL;
	ComPoint* interfaceOut = NULL;

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
						input = (IncomingMsg*)receiveQueue.back();
						log(logInfoIn, input->getContent());
						popReceiveQueueWithoutDelete();
						output = pInterface->processMsg(input);

						if(output != NULL)//output can be 0 if we received a notification, so there will be no response !
						{
							interfaceOut = output->getComPoint();
							if(interfaceOut != NULL)
								interfaceOut->transmit(output);
							else
							{
								//TODO: error
							}
							delete output;
						}
					}
					catch(Error &e)
					{
						dlog(logInfo, "Catched instance of error: %s\n", e.get());
					}
					catch(...)
					{
						log(logInfo, "Catched unkown Exception.\n");
					}
				}
			break;

			default:
				log(logInfo, "Received unkown/not supported signal \n");
				break;
		}
	}
	close(currentSocket);
}



void ComPoint::thread_listen()
{
	listen_thread_active = true;
	int recvSize = 0;
	int messageSize = 0;
	string* content = NULL;
	pthread_t worker_thread = getWorker();

	while(listen_thread_active)
	{
		memset(receiveBuffer, '\0', BUFFER_SIZE);

		recvSize = recv(currentSocket , receiveBuffer, BUFFER_SIZE, 0);
		if(recvSize > 0)
		{
			try{
				//copy receiveBuffer to a clean msgBuffer
				msgBufferSize = recvSize;
				msgBuffer = new char[msgBufferSize];
				memset(msgBuffer, '\0', msgBufferSize);
				memcpy(msgBuffer, receiveBuffer, msgBufferSize); // [data]

				//As long as there is data in the msgBuffer
				while(msgBufferSize > 0)
				{
					if(msgBufferSize > HEADER_SIZE)
					{
						//try to decode the header
						messageSize = readHeader(msgBuffer);

						if(messageSize > -1)
						{
							//Is there at least one complete message in msgBuffer ?
							if(msgBufferSize >= messageSize+HEADER_SIZE)
							{
								//add first complete msg of msgbuffer to the receivequeue and signal the worker
								content = new string(&msgBuffer[HEADER_SIZE], messageSize);


								push_frontReceiveQueue(new IncomingMsg(this, content));
								pthread_kill(worker_thread, SIGUSR1);

								//Is there more data ? Maybe another message ?
								if(msgBufferSize > messageSize+HEADER_SIZE)
								{
									//copy rest of data to a new clean msgBuffer  [alreadyInQueueMsg | rest of data]
									msgBufferSize = msgBufferSize - (messageSize+HEADER_SIZE);
									tempBuffer = new char[msgBufferSize];
									memset(tempBuffer, '\0', msgBufferSize);
									memcpy(tempBuffer, &(msgBuffer[messageSize+HEADER_SIZE]), msgBufferSize);
									delete[] msgBuffer;
									msgBuffer = tempBuffer; //[rest of data]
									tempBuffer = NULL;
								}
								//no further data, delete msgBuffer
								else
								{
									delete[] msgBuffer;
									msgBuffer = NULL;
									msgBufferSize = 0;
								}
							}
							//message is not complete, wait for the rest
							else
							{
								waitForFurtherData();
							}
						}
						//Decoding the header failed
						else
						{
							delete[] msgBuffer;
							msgBuffer = NULL;
							msgBufferSize = 0;
							transmit(JSON_ERROR_RESPONSE_INCORECCT_MSG, strlen(JSON_ERROR_RESPONSE_INCORECCT_MSG));
						}
					}
					//not even the header was complete, wait for the rest
					else
					{
						waitForFurtherData();
					}
				}
			}
			catch(Error &e)
			{
				delete[] msgBuffer;
				msgBuffer = NULL;
				msgBufferSize = 0;
			}
		}
		//Error with recv, close listener
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


int ComPoint::transmit(OutgoingMsg* output)
{
	int sendCount = 0;
	int msgSize = output->getContent()->size();
	string* msgContent = output->getContent();
	log(logInfoOut, msgContent);

	createHeader(headerOut, msgSize);
	sendCount = send(currentSocket, headerOut, HEADER_SIZE, 0);
	sendCount += send(currentSocket, msgContent->c_str(), msgSize, 0);
	return sendCount;
};


void ComPoint::createHeader(char* header,  int value)
{
	memset(header, '\0', HEADER_SIZE);
	header[0] = (char)93; // magic number for identifying the header
	if(value > 0)
	{
		for(int i = HEADER_SIZE-1 ; i > 0 ; i--)
		{
			header[i] = (char)value;
			value = value >> 8;
		}
	}
}


unsigned int ComPoint::readHeader(char* buffer)
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


void ComPoint::waitForFurtherData()
{
	int recvSize = 0;
	int oldMsgBufferSize = 0;

	//wait for more with select and timeout
	int retval = select(currentSocket+1, &rfds, NULL, NULL, &timeout);
	if(retval > 0)
	{
		recvSize = recv(currentSocket , receiveBuffer, BUFFER_SIZE, 0);
		if(recvSize < 0)
			throw Error("Error while waiting for further data.");

		//save the size of the currentMsgbuffer, allocate a bigger buffer for old+ new data
		oldMsgBufferSize = msgBufferSize;
		msgBufferSize = msgBufferSize + recvSize;
		tempBuffer = new char[msgBufferSize];
		memset(tempBuffer, '\0', msgBufferSize);

		//copy old data, then copy new data to new buffer, delete old one
		memcpy(tempBuffer, msgBuffer, oldMsgBufferSize);
		memcpy(&tempBuffer[oldMsgBufferSize], receiveBuffer, recvSize);
		deleteBuffer(msgBuffer);
		msgBuffer = tempBuffer;
		tempBuffer = NULL;
	}
	else if(retval == 0)
	{
		deleteBuffer(msgBuffer);
		msgBufferSize = 0;
		msgBuffer = NULL;
		throw Error("Timeout while waiting for further data.");
	}
	else
	{
		//errno prüfen
		//error msg senden
		throw Error("Error while waiting for further data.");
	}
}


void ComPoint::deleteReceiveQueue()
{
	list<RPCMsg*>::iterator i;
	pthread_mutex_lock(&rQmutex);
	if(receiveQueue.size() > 0) //TODO:
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


void ComPoint::popReceiveQueue()
{
	RPCMsg* lastElement = NULL;
	pthread_mutex_lock(&rQmutex);
	lastElement = receiveQueue.back();
	receiveQueue.pop_back();
	delete lastElement;
	pthread_mutex_unlock(&rQmutex);
}


void ComPoint::popReceiveQueueWithoutDelete()
{
	pthread_mutex_lock(&rQmutex);
		receiveQueue.pop_back();
	pthread_mutex_unlock(&rQmutex);
}


void ComPoint::push_backReceiveQueue(RPCMsg* data)
{
	pthread_mutex_lock(&rQmutex);
		receiveQueue.push_back(data);
	pthread_mutex_unlock(&rQmutex);
}


int ComPoint::getReceiveQueueSize()
{
	int result = 0;
	pthread_mutex_lock(&rQmutex);
		result = receiveQueue.size();
	pthread_mutex_unlock(&rQmutex);
	return result;
}


void ComPoint::configSignals()
{
	sigemptyset(&origmask);
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGUSR1);
	sigaddset(&sigmask, SIGUSR2);
	pthread_sigmask(SIG_BLOCK, &sigmask, &origmask);
}


void ComPoint::deleteBuffer(char* buffer)
{
	if(buffer != NULL)
	{
		delete[] buffer;
		buffer = NULL;
	}
}
