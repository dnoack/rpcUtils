#ifndef PLUGININTERFACE_HPP_
#define PLUGININTERFACE_HPP_

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <list>
#include <pthread.h>
#include "signal.h"

#include "AcceptThread.hpp"
#include "LogUnit.hpp"
#include "ComPoint.hpp"
#include "PluginInfo.hpp"
#include "RegClient.hpp"

/*! Wait time in seconds for main poll loop.*/
#define WAIT_TIME 3
/*! Max number of concurrent clients connecting to a plugin.*/
#define MAX_CLIENTS 20

using namespace std;

/**
 * \interface PluginInterface
 * Represents the common interface for every Plugin of Remote-Server-Daemon.
 * Plugins always have to:
 * - Implement thread_accept, where it create other class instances related to the Plugin (hw driver class or something like this).
 * - Constructed with a PluginInfo object (\see PluginInfo).
 * - Started with start() after the object construction.
 * \note Because of inheriting from AcceptThread, the thread_accept() function will later run in a separate thread.
 */
class PluginInterface : public AcceptThread, public LogUnit{

	public:

		/**
		 * Constructor.
		 * \param plugin Have to contain name, number and file path for communication of the plugin.
		 */
		PluginInterface(PluginInfo* plugin);

		/** Base-Destructor.*/
		virtual ~PluginInterface();

		/**
		 * Start the plugin. This means, it tries to register to RSD and after that executing a loop
		 * where it constantly searches for deletable ComPoints.
		 */
		void start();


		/** Searches for inactive ComPoints and deletes them. */
		void checkForDeleteableComPoints();

	protected:

		 /*! Socket for receiving new incoming IPC connections.*/
		 int connection_socket;
		 /*! Unique plugin number.*/
		 int pluginNumber;
		 /*! Unique plugin name.*/
		 string* pluginName;
		 /*! File path for Unix domain socket file.*/
		 string* pluginPath;

		 /*!LogInformation for incoming messages of underlying ComPoints.*/
		LogInformation infoIn;
		 /*!LogInformation for outgoing messages of underlying ComPoints.*/
		LogInformation infoOut;
		 /*!LogInformation for underlying ComPoints.*/
		LogInformation info;

		/*! Contains all created ComPoints except the one for registration.*/
		list<ComPoint*> comPointList;
		/*! Protects comPointList .*/
		pthread_mutex_t cpLMutex;

		/*! Optionflag for accept_socket.*/
		int optionflag;
		/*! Unix domain socket address.*/
		struct sockaddr_un address;
		/*! Size of Unix domain socket address.*/
		socklen_t addrlen;


		/*! Handles the registry process to RSD.*/
		RegClient* regClient;

		/*! Signalmask for whole plugin.*/
		sigset_t sigmask;
		/*! Old signal mask of plugin.*/
		sigset_t origmask;

		/*! As long as this flag is true, the server will run his mainloop in start().*/
		bool pluginActive;

		/**
		 * Adds a ComPoint to the list of ComPoints.
		 * \param newWorker A new created and already connected ComPoint to RSD.
		 */
		void pushComPointList(ComPoint* newWorker);

		/**
		 * Deallocates all ComPoints within the list of ComPoints and the list itself.
		 */
		void deleteComPointList();

		/**
		 * Has to accept new connections and create a new instance of a class which implements ProcessInterface(B).
		 * After that it has to use pushComPointList to add the new ComPoint to the list of all ComPoints.
		 * \pure
		 */
		virtual void thread_accept() = 0;
};

#endif /* PLUGININTERFACE_HPP_ */
