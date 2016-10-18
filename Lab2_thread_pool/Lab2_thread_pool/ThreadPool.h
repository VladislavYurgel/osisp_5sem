#include "Thread.h"
#pragma once
class ThreadPool
{
public:
	Thread *Threads[15];
	DWORD ThreadCount;
	DWORD tasksCount;

	ThreadPool(DWORD threadCount);
	~ThreadPool();

	void InitializePool();
	void AssignTask(LPTHREAD_START_ROUTINE method, char* methodName);
	DWORD SearchFreeThread();
	void WaitAllThreads();
};

