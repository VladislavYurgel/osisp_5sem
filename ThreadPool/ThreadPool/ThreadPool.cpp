#include "MyThread.h"

#define THREADS_NUMBER 10

HANDLE hThreads[THREADS_NUMBER];

ThreadPool::ThreadPool()
{
	freeThreadCount = THREADS_NUMBER;
}

void ThreadPool::CreateTask(LPTHREAD_START_ROUTINE method)
{
	if (freeThreadCount > 0)
	{
		freeThreadCount--;
		MyThread *newThread = new MyThread(method);
		printf("Free thread: %d\r\n", freeThreadCount);
	}
}

ThreadPool::~ThreadPool()
{

}
