#ifndef INCLUDE_LOGUNIT_HPP_
#define INCLUDE_LOGUNIT_HPP_

#include <cstdio>
#include <stdarg.h>
#include <time.h>
#include <cstring>
#include <string>
#include <syslog.h>

#include "Error.hpp"

/*! Bitmask for disable logging.*/
#define _LOG_OFF 0
/*! Bitmask for loglevel 1.*/
#define _LOG_INPUT 1
/*! Bitmask for loglevel 2.*/
#define _LOG_OUTPUT 2
/*! Bitmask for loglevel 3.*/
#define _LOG_INFO 4
/*! Bitmask for log messages from all loglevels.*/
#define _LOG_ALL 7

/*! Constant for logMethod, enables logging to console with printf.*/
#define PRINTF_LOG 0
/*! Constant for logMethod, enables logging with syslog.*/
#define SYSLOG_LOG 1

/** Standard max. number of characters in one line.*/
#define LINESIZE 80
/*! Maximumg number of characters for nesting messages.*/
#define DLOG_BUFFERSIZE 4096
/*! Size of Buffer for creating timestamps.*/
#define TIMESTRING_BUFFERSIZE 64

using namespace std;

/** Contains information about if and how to log a message.*/
typedef struct LogInformation
{
	/*! Prefix which will set right in front of the log message.*/
	const char* logName;
	/*! Loglevel which will be compared to the set global log level.*/
	int logLevel;
}
LogInformation;

/**
 * \class LogUnit
 * LogUnit is a class for logging. For using it, a class has to inherit from it. LogUnit has a 3 Bit
 * logmask where:
 * - 000 = 0 means no Logging
 * - 001 = 1 is a loglevel and is normally used for logging incoming messages
 * - 010 = 2 is a loglevel and is normally used for logging outgoing messages.
 * - 100 = 4 is a loglevel and is normally used for information about action within a class which has nothing to do with in/outgoin messages.
 * - 111 = 7 which means log all logLevels.
 * By default, the global log level is set to LOG_ALL ( which is 7). Every class which want to log with LogUnit should initialize
 * the struct logInfo with a logName and logLevel. For information about in- or outgoing messages, you have to add some LogInformation
 * structs to your class. Everytime you want to log a message, you need a Loginformation and the message itself. LogUnit will compare the
 * loglevel of your given information and the already set globalLoglevel. If the bit of your Loginformation is set within the globalLoglevel, the
 * message will be logged, otherwise it will not.
 */
class LogUnit
{

	public:
		/** Base-Constructor, setting loglevel of logInfo to LOG_INFO (4) and
		 * the max number of characters per line to 80.*/
		LogUnit(int logMethod = PRINTF_LOG, int syslogFacility = LOG_LOCAL0)
		{
			logInfo.logLevel = _LOG_INFO;
			logInfo.logName = ":";
			lineSize = LINESIZE;
			this->logMethod = logMethod;
			this->syslogFacility = syslogFacility;
		}

		/** Base Destructor.*/
		virtual ~LogUnit(){};

		/**
		 * Changes the global loglevel.
		 * \param level The level that should be set.
		 * \return The old loglevel.
		 * \throws Error If level is not a valid value.
		 * \note valid values for loglevel are 0,1,2,4 and 7.
		 */
		int setGlobalLogLevel(int level)
		{
			int oldLevel = globalLogLevel;
			if(level != _LOG_OFF || level != _LOG_INPUT || level != _LOG_OUTPUT
				|| level != _LOG_INFO || level != _LOG_ALL)
			{

				globalLogLevel = level;
			}
			else
				throw Error("Invalid loglevel");
			return oldLevel;
		}

		static void setGlobalLogMethod(int method){globalLogMethod = method;}
		static int getGlobalLogMethod(){return globalLogMethod;}

		/**
		 * Sets the maximum number of characters in one line.
		 * \param lineSize Number of characters.
		 */
		void setLineSize(int lineSize){this->lineSize = lineSize;}

		/** Sets the logMethod either to printf or syslog (see defines).
		 * \param logMethod Should be 0 for printf and 1 for syslog.
		 */
		void setLogMethod(int logMethod){this->logMethod = logMethod;}

		/**
		 * Sets the facility for facility part of the syslog priority parameter.
		 * \param facility Facility constant from syslog.h
		 */
		void setSyslogFacility(int facility){this->syslogFacility = facility;}


	protected:

		/*! Every class which inherits from LogUnit got at least this LogInformation struct for logging messages.*/
		LogInformation logInfo;
		/*! Contains max. number of characters in one line.*/
		int lineSize;
		/*! Specifies the method how the message should be logged.*/
		int logMethod;
		/*! Specifies the syslog facility for logging.*/
		int syslogFacility;


		/**
		 * Compares logLevel of the given parameter and globalLogLevel and logs
		 * the message if the given logLevel is smaller or equal globalLogLevel.
		 * \param info Containing Logprefix and loglevel of the message.
		 * \param msg The message which should be logged.
		 */
		void log(LogInformation info, string* msg)
		{
			if(info.logLevel == (info.logLevel & globalLogLevel))
			{
				string* logMsg = new string(*(msg));
				_log(info.logName, logMsg);
			}
		}


		/**
		 * Compares logLevel of the given parameter and globalLogLevel and logs
		 * the message if the given logLevel is smaller or equal globalLogLevel.
		 * \param info Containing Logprefix and loglevel of the message.
		 * \param msg The message which should be logged.
		 */
		void log(LogInformation info, const char* msg)
		{
			if(info.logLevel == (info.logLevel & globalLogLevel))
			{
				string* logMsg = new string(msg);
				_log(info.logName, logMsg);
			}
		}


		/**
		 * Compares logLevel of the given parameter and globalLogLevel and logs
		 * the message if the given logLevel is smaller or equal globalLogLevel. This method
		 * supports nesting message like printf.
		 * \param info Containing Logprefix and loglevel of the message.
		 * \param msg The message which should be logged, printf-syntax can be used.
		 */
		void dlog(LogInformation info, const char* msg, ...)
		{
			char buffer[DLOG_BUFFERSIZE];
			string* logMsg = NULL;

			if(info.logLevel == (info.logLevel & globalLogLevel))
			{
				memset(buffer, '\0', DLOG_BUFFERSIZE);
				va_list arguments;

				va_start(arguments, msg);
				vsprintf(buffer, msg, arguments);
				logMsg = new string(buffer);
				_log(info.logName, logMsg);
				va_end(arguments);
			}
		}

	private:
		 /*! Global loglevel which defines the threshold value for logging.*/
		 static int globalLogLevel;
		 static int globalLogMethod;

		 /**
		 * Internal method for logging messages. The message is formated to the
		 * defined max number of characters per line. Also a timestamp and the
		 * logging prefix is added.
		 * \param logName Prefix for the log message.
		 * \param logMsg The log message itself.
		 */
		void _log(const char* logName, string* logMsg)
		{
			switch(logMethod)
			{
				case PRINTF_LOG:
					formatString(strlen(logName), &logMsg);
					logMsg->replace(0, strlen(logName), logName);
					getTimeStamp(&logMsg);
					dyn_print("%s", logMsg->c_str());
					break;
				case SYSLOG_LOG:
					logMsg->insert(0, strlen(logName), ' ');
					logMsg->replace(0, strlen(logName), logName);
					syslog(syslogFacility | LOG_NOTICE, "%s", logMsg->c_str());
					break;
			}
			delete logMsg;
		}


		/**
		 * Prints log messages to std::out. A variable number of messages can be printed with one
		 * call like printf.
		 * \param data Message which should be printed.
		 * \return Number of characters which where printed.
		 */
		int dyn_print(const char* data, ...)
		{
			int printed = 0;

			va_list arguments;
			va_start(arguments, data);
			printed = vprintf(data, arguments);
			va_end(arguments);
			return printed;
		}


		/**
		 * Formats a give string, so it does not exceed the maximum number of characters
		 * per line. There will be no new string allocated, instead an existing string will
		 * be edited.
		 * \param offset Maximum number of characters per line.
		 * \param input Pointer to a pointer of the string which should be formated.
		 */
		void formatString(int offset, string** input)
		{
			string* data = *input;
			int temp = data->size();
			int newlineCount = temp / (lineSize-offset);
			int size = newlineCount*(lineSize+offset);

			data->insert(0, offset, ' ');

			for(int i = lineSize ; i <= size ; i+= lineSize)
			{
				data->insert(i, "\n");
				data->insert(i+1, offset, ' ');
			}
			data->append("\n\n");
		}


		/**
		 * Adds one line with a timestamp to a given string.
		 * The given string will be edited.
		 * \param input String which should get the timestamp.
		 */
		void getTimeStamp(string** input)
		{
			struct tm timeStruct;
			string* data = *input;
			time_t currentTime = time(NULL);
			char buffer[TIMESTRING_BUFFERSIZE];
			memset(buffer, '\0', TIMESTRING_BUFFERSIZE);

			timeStruct = *localtime(&currentTime);
			strftime(buffer, TIMESTRING_BUFFERSIZE, "%d.%m.%Y-%T", &timeStruct);
			data->insert(0, 31, '-');
			data->insert(31, buffer, 19);
			data->insert(50, 30, '-');
			data->insert(80, "\n");
		}
};


#endif /* INCLUDE_LOGUNIT_HPP_ */
