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
#define MAX_CLIENTS 20

using namespace std;

class PluginInterface : public AcceptThread, public LogUnit{

	public:
		PluginInterface(PluginInfo* plugin);
		virtual ~PluginInterface();

		/**
		 * Start the plugin. This means, it tries to register to RSD and after that executing a loop
		 * where it constantly searches for deletable worker.
		 */
		void start();

		void checkForDeletableWorker();

	protected:

		 int connection_socket;
		 int pluginNumber;
		 string* pluginName;
		 string* pluginPath;

		 /*!LogInformation for underlying ComPoints.*/
		LogInformation infoIn;
		LogInformation infoOut;
		LogInformation info;

		list<ComPoint*> comPointList;
		pthread_mutex_t wLmutex;

		int optionflag;
		struct sockaddr_un address;
		socklen_t addrlen;


		/*! Handles the registry process to RSD.*/
		RegClient* regClient;

		sigset_t sigmask;
		sigset_t origmask;

		/*! As long as this flag is true, the server will run his mainloop in start().*/
		bool pluginActive;

		void pushComPointList(ComPoint* newWorker);
		void deleteComPointList();


		virtual void thread_accept() = 0;
};

#endif /* PLUGININTERFACE_HPP_ */
