#include "Log.h"

const TCHAR Log::logFileName[] = L"log.dat";
FILE *Log::file = fopen("log.dat", "a+");

Log::Log()
{
}

Log::~Log()
{
}

void Log::LogPrint(char * messageText)
{
	time_t now = time(0);
	char *dt = ctime(&now);
	tm *gmtm = gmtime(&now);
	dt = asctime(gmtm);
	dt[strlen(dt) - 1] = '\0';

	printf("[%s] %s", dt, messageText);

	fprintf(Log::file, "[%s] %s", dt, messageText);
}