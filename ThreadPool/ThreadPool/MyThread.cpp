#include "MyThread.h"

DWORD ThreadPool::freeThreadCount;

MyThread::MyThread(LPTHREAD_START_ROUTINE method)
{
	currentThread = CreateThread(NULL, 0, method, NULL, 0, NULL);
	printf("Thread was started!\r\n");
	ExecuteMethod();
}

void MyThread::ExecuteMethod()
{
	//DWORD waitAnswer = WaitForSingleObject(currentThread, INFINITE);
	DWORD waitAnswer;
	GetExitCodeThread(currentThread, &waitAnswer);

	/*switch (waitAnswer)
	{
		case WAIT_OBJECT_0:
		{
			DestroyMyThread();
			break;
		}
		case WAIT_TIMEOUT:
		{
			DestroyMyThread();
			break;
		}
		case WAIT_ABANDONED:
		{
			DestroyMyThread();
			break;
		}

		// delete thread if they has finished the method
	}*/
}

void MyThread::DestroyMyThread()
{
	TerminateThread(currentThread, 0);
	ThreadPool::freeThreadCount++;
	printf("Thread has been destoroyed\r\nFree thread count: %d\r\n", ThreadPool::freeThreadCount);
}

MyThread::~MyThread()
{
}