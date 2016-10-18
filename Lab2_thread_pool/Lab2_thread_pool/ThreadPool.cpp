#include "ThreadPool.h"

ThreadPool::ThreadPool(DWORD count)
{
	ThreadCount = count;
	tasksCount = 0;
}

ThreadPool::~ThreadPool()
{
}

void ThreadPool::InitializePool()
{
	for (DWORD i = 0; i < ThreadCount; i++)
	{
		Threads[i] = new Thread(i);
	}
	char messageBuff[100];
	snprintf(messageBuff, sizeof(messageBuff), "Successful! Inzialized %d threads\n", ThreadCount);
	Log::LogPrint(messageBuff);
}

void ThreadPool::AssignTask(LPTHREAD_START_ROUTINE method, char* methodName)
{
	INT freeThreadID;
	BOOL firstEnter = TRUE;
	while ((freeThreadID = SearchFreeThread()) < 0)
	{
		if (firstEnter)
		{
			Log::LogPrint("All threads is busy, wait...\n");
			firstEnter = FALSE;
		}
	}
	/*if ((freeThreadID = SearchFreeThread()) < 0)
	{
		Log::LogPrint("Error! All threads are busy...\n");
		return;
	}*/

	firstEnter = TRUE;
	Threads[freeThreadID]->inThread.methodName = methodName;
	Threads[freeThreadID]->RunTask(method);
}

DWORD ThreadPool::SearchFreeThread()
{
	for (DWORD i = 0; i < ThreadCount; i++)
	{
		if (Threads[i]->IsFree())
			return i;
	}

	return -1;
}

void ThreadPool::WaitAllThreads()
{
	int freeThreadsCount = 0;
	while (freeThreadsCount != ThreadCount)
	{
		freeThreadsCount = 0;
		for (int i = 0; i < ThreadCount; i++)
		{
			if (Threads[i]->IsFree())
				freeThreadsCount++;
		}
	}

	Log::LogPrint("Successful! All threads are destroyed!\n");
}


