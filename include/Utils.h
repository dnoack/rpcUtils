#include <cstdio>
#include <stdarg.h>
#include <time.h>
#include <cstring>
#include <string>

#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

using namespace std;

#define LINESIZE 80
#define TIMESTRING_BUFFERSIZE 64


static inline int dyn_print(const char* data, ...)
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

static inline void formatString(int offset, string** input)
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


static inline void getTimeStamp(string** input)
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



#endif /* INCLUDE_UTILS_H_ */
