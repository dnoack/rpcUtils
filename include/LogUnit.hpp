
#ifndef INCLUDE_LOGUNIT_HPP_
#define INCLUDE_LOGUNIT_HPP_

#include <cstdio>
#include <stdarg.h>
#include <time.h>
#include <cstring>
#include <string>

#define DLOG_BUFFERSIZE 4096
#define LOG_OFF 0
#define LOG_INPUT 1
#define LOG_OUTPUT 2
#define LOG_INFO 4
#define LOG_ALL 7
#define LINESIZE 80
#define TIMESTRING_BUFFERSIZE 64

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
			globalLogLevel = 7;
		}

		virtual ~LogUnit(){};


		 int setGlobalLogLevel(int level)
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


		int dyn_print(const char* data, ...)
		{
			int printed = 0;

			#ifdef DEBUG
				va_list arguments;
				va_start(arguments, data);
				printed = vprintf(data, arguments);
				va_end(arguments);
			#endif
				//there can be other print-like functions

			return printed;
		}

	void formatString(int offset, string** input)
	{
		string* data = *input;
		int temp = data->size();
		int newlineCount = temp / LINESIZE;
		int size = newlineCount*LINESIZE;

		data->insert(0, offset, ' ');

		for(int i = LINESIZE ; i <= size ; i+= LINESIZE)
		{
			data->insert(i, "\n");
			data->insert(i+1, offset, ' ');
		}
		data->append("\n\n");
	}


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

	private:
		 int globalLogLevel;
};



#endif /* INCLUDE_LOGUNIT_HPP_ */
