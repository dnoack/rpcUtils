
#ifndef INCLUDE_LOGUNIT_HPP_
#define INCLUDE_LOGUNIT_HPP_

#include <string>
#include "Utils.h"

#define DLOG_BUFFERSIZE 4096
#define LOG_OFF 0
#define LOG_INPUT 1
#define LOG_OUTPUT 2
#define LOG_INFO 4
#define LOG_ALL 7

using namespace std;

typedef struct LogInformation
{
	const char* logName;
	int logLevel;
}
LogInformation;


class LogUnit
{

	public:
		LogUnit()
		{
			logInfo.logLevel = LOG_INFO;
			logInfo.logName = ":";
		}

		virtual ~LogUnit(){};


		static int setGlobalLogLevel(int level)
		{
			int oldLevel = globalLogLevel;
			globalLogLevel = level;
			return oldLevel;
		}


	protected:

		LogInformation logInfo;

		void log(LogInformation info, string* msg)
		{
			if(info.logLevel == (info.logLevel & globalLogLevel))
			{
				string* logMsg = new string(*(msg));
				_log(info.logName, logMsg);
			}
		}

		void log(LogInformation info, char* msg)
		{
			if(info.logLevel == (info.logLevel & globalLogLevel))
			{
				string* logMsg = new string(msg);
				_log(info.logName, logMsg);
			}
		}

		void log(LogInformation info, const char* msg)
		{
			if(info.logLevel == (info.logLevel & globalLogLevel))
			{
				string* logMsg = new string(msg);
				_log(info.logName, logMsg);
			}
		}


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


		void _log(const char* logName, string* logMsg)
		{
			formatString(strlen(logName), &logMsg);
			logMsg->replace(0, strlen(logName), logName);
			getTimeStamp(&logMsg);
			dyn_print("%s", logMsg->c_str());
			delete logMsg;
		}

	private:
		static int globalLogLevel;
};



#endif /* INCLUDE_LOGUNIT_HPP_ */
