#ifndef COMPOINT_HPP_
#define COMPOINT_HPP_

#define JSON_ERROR_RESPONSE_INCORECCT_MSG "{\"jsonrpc\": \"2.0\", \"error\": {\"code\":  -1, \"message\": \"Received message with invalid packet encoding.\"}, \"id\": 0}"

#include <list> //receiveQueue
#include <unistd.h> //close
#include <sys/socket.h> //send, recv
#include <sys/select.h> //waitForFurtherData
#include <pthread.h>
#include "signal.h"


#include "WorkerThreads.hpp"
#include "LogUnit.hpp"
#include "ProcessInterface.hpp"
#include "Error.hpp"
#include "RPCMsg.hpp"
#include "OutgoingMsg.hpp"

/*! Size of ReceiveBuffer in byte */
#define BUFFER_SIZE 2048
/*! Size of TAG-field in byte*/
#define TAG_LENGTH  1
/*! Size of LENGTH-field in byte*/
#define LENGTH_LENGTH  4
/*! Size of complete Header in byte*/
#define HEADER_SIZE TAG_LENGTH + LENGTH_LENGTH
/*! Time for receiving a whole encoded message until timeout.*/
#define RECV_TIMEOUT 3

class ProcessInterface;
class RPCMsg;
class OutgoingMsg;


/**
 * \class ComPoint
 * ComPoint uses the class WorkerThreads to split its functionality into two separated threads thread_listen and thread_work.
 * Thread_listen (listener) receives data through a socket and tries to decode the content of it. After a successful decode, it
 * creates a IncomingMsg containing a JSON RPC, pushes this message into the receiveQueue and signals thread_work (worker)
 * about this event. The worker then calls processMsg(IncomingMsg*) method of the processInterface (which is delivered through the
 * constructor). If ProcessMsg responds with a OutgoingMsg, this answer will be send over the socket of this ComPoint.
 */
class ComPoint :  public WorkerThreads, public LogUnit{

	public:

		/** Base-Constructor*/
		ComPoint();

		/**
		 * \param socket The socket which should be used for receive and transmit data.
		 * \param pInterface A instance of a class which inherits from ProcessInterface and implements the processMsg() method.
		 * \param uniqueID Unique identifier which should be 0 for RSD front and greater 0 for a plugin.
		 * \param startInstant Optional parameter, normally true. If false, the worker and listener thread will not be started within the constructor.
		 * \throws Error If the listener is not started during a defined time (defined by WorkerThread.hpp).
		 */
		ComPoint(int socket, ProcessInterface* pInterface, int uniqueID, bool startInstant = true);

		/** Base-Destructor
		 * If there are still active worker/listener threads, they will be stopped and the socket will be closed.
		 */
		virtual ~ComPoint();

		/**
		 * Sets the prefix of incoming, outgoing and informal log messages.
		 * \param in Contains information about the log prefix for incoming messages.
		 * \param out Contains information about the log prefix for outgoing messages.
		 * \param info Contains information about the log prefix for informal messages.
		 */
		void configureLogInfo(LogInformation* in, LogInformation* out, LogInformation* info );

		/** Starts thread_work and thread_listen if not already started.*/
		void startWorking()
		{
			if(getWorker() == 0 && getListener() == 0)
				StartWorkerThread();

			if(wait_for_listener_up() != 0)
				throw Error("Creation of Listener/worker threads failed.");
		}

		/**
		 * \return The corresponding ProcessInterface, which where delivered by the constructor.
		 */
		ProcessInterface* getProcessInterface(){return pInterface;}

		/**
		 * \return The unique ID of this ComPoint.
		 */
		int getUniqueID(){return this->uniqueID;}

		/**
		 * Transmits data through the corresponding socket of this ComPoint. The data will be encoded
		 * before it is transmitted. Encoded data contains a additional header of tag + length field.
		 * \param data The data which should be transmitted.
		 * \param size The size of the data.
		 * \return The number of transmitted bytes (should be size of data + size of header).
		 */
		int transmit(const char* data, int size);

		/**
		 * Transmits data through the corresponding socket of this ComPoint. The data will be encoded
		 * before it is transmitted. Encoded data contains a additional header of tag + length field.
		 * \param output The data which should be transmitted.
		 * \return The number of transmitted bytes.
		 */
		int transmit(OutgoingMsg* output);

		/**
		 * \return True if this ComPoint is not active anymore (sockets closed), false else.
		 */
		bool isDeletable(){return deletable;}


		//sometimes a processInterface has to analyse data and then put it in queue, so we need it public
		/**
		 * Pushes a RPCMsg to the intern ReceiveQueue, which is a list. The message will be pushed at the front
		 * of the list.
		 * \param data The message which should be pushed to the ReceiveQueue.
		 * \note The pushed message must not be manually deleted.
		 */
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
		/*! ReceiveBuffer of thread_listen for receiving data over a socket.*/
		char receiveBuffer[BUFFER_SIZE];
		/*! Dynamic buffer for receiving at least one whole encoded message.*/
		char* msgBuffer;
		/*! Size of msgBuffer.*/
		int msgBufferSize;
		/*! Dynamic buffer for copying split encoded messages and to increase the msgBuffer.*/
		char* tempBuffer;
		/*! Contains the header of outgoing encoded messages.*/
		char* headerOut;

		/*! Set of Receiving File DescriptorS, which contains the socket for receiving data. This is needed
		 for select() in waitForFurtherData().*/
		fd_set rfds;
		/*! Defines how long we wait for further data of a encoded message until we trigger a timeout.*/
		struct timeval timeout;

		/*! The socket of this worker, which is used to send and receive data.*/
		int currentSocket;

		//signal variables
		sigset_t sigmask;
		sigset_t origmask;
		int currentSig;

		/*! Flag which signals if this ComPoint is not used anymore.*/
		bool deletable;

		/*! Contains Log-information for logging incomming data.*/
		LogInformation logInfoIn;
		/*! Contains Log-information for logging outgoing data.*/
		LogInformation logInfoOut;

		/**
		 * Receives data through the corresponding socket and tries to decode the data.
		 * If a message is not transmitted completely with one recv(), it uses waitForFurtherData()
		 * to try to receive the rest of it in a defined time. After a successful reception the message
		 * is saved as IncommingMsg and pushed to the receiveQueue for the worker.
		 * \note This function works in a separate thread.
		 */
		virtual void thread_listen();

		/**
		 * Waits for a signal of the listener (thread_listen) and processes all messages of the receiveQueue.
		 * As long as new messages arrives during a processing, the worker will automatically process the next message.
		 * For processing a message, the method processMsg() of the corresponding ProcessInterface is used. A call of
		 * processMsg() can generate a answer, which then will be transmitted over the corresponding socket. If processMsg()
		 * does some asynchronous work and doesn't generate a direct response it will return with NULL, resulting in process the
		 * next message in the worker thread.
		 */
		virtual void thread_work();

		/**
		 * If a encoded message is not completely received with one call of recv(), this method can be used for waiting for missing
		 * data until a defined timeout. If data is received, the internal buffer will be increased and the old data + the new data
		 * will be copied into the new bigger buffer. Afterwards the listener can try again to decode the message.
		 * \throws Error If the socket is closed during waiting or if a timeout occured.
		 */
		void waitForFurtherData();

		/**
		 * Creates the header data for a message which should be encoded.
		 * Header contains 1 Byte header tag and a 4 Byte length tag of the transported message.
		 * \param header Pointer to a array which has to be 5 bytes big (use headerOut):
		 * \param value Size of the message which should be encoded.
		 */
		void createHeader(char* header,  int value);

		/**
		 * \param buffer The buffer which contains a encoded header at the beginning.
		 * \return The size of the encoded message.
		 */
		unsigned int readHeader(char* buffer);

	private:

		/*! The corresponding object of a class which inherits from ProcessInterface.
		 * The worker thread will use this to process queued messages.
		 */
		ProcessInterface* pInterface;

		/**
		 * Pops all messages of the receiveQueue and deallocates every message.
		 * \note This function is protected by the mutex rQmutex.
		 */
		void deleteReceiveQueue();

		/**
		 * Pops a message at the end of the receiveQueue WITH deallocating the
		 * message itself.
		 * \note This function is protected by the mutex rQmutex.
		 */
		void popReceiveQueue();

		/**
		 * Pops a message at the end of the receiveQueue WITHOUT deallocating the
		 * message itself.
		 * \note This function is protected by the mutex rQmutex.
		 */
		void popReceiveQueueWithoutDelete();

		/**
		 * Pushes a RPCMsg at the end of the receiveQueue.
		 * \param rpcMSg Instance of RPCMsg or IncommingMsg.
		 * \note This function is protected by the mutex rQmutex.
		 */
		void push_backReceiveQueue(RPCMsg* rpcMsg);

		/**
		 * \return The Size of the receiveQueue.
		 * \note This function is protected by the mutex rQmutex.
		 */
		int getReceiveQueueSize();

		/**
		 * Configures the signals SIGUSR1 and SIGUSR2 for using in
		 * thread_listen and thread_work.
		 */
		void configSignals();

		/**
		 * Little helper function which checks a pointer of a arry and deallocates it.
		 * \param buffer A pointer to a valid array or NULL.
		 */
		void deleteBuffer(char* buffer);

};

#endif /* COMPOINT_HPP_ */
