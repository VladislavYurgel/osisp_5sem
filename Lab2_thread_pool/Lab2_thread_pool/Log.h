#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

#pragma once
class Log
{
public:
	Log();
	~Log();
	static FILE *file;
	static const TCHAR logFileName[];
	static void LogPrint(char* messageText);
};

