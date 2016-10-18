#include "Thread.h"

Thread::Thread(DWORD threadId)
{
	inThread.isFree = TRUE;
	inThread.threadId = threadId;
}

Thread::~Thread()
{
}

unsigned int __stdcall Thread::ThreadProc(in* inThreadStruct)
{
	DWORD dwWaitResult;
	GetExitCodeThread(inThreadStruct->hThread, &dwWaitResult);

	while (dwWaitResult == STILL_ACTIVE)
	{
		GetExitCodeThread(inThreadStruct->hThread, &dwWaitResult);
	}

	char messageBuff[100];
	snprintf(messageBuff, sizeof(messageBuff), "Thread with id %d has been ended execute method \"%s\"\n", inThreadStruct->threadId, inThreadStruct->methodName);
	Log::LogPrint(messageBuff);

	snprintf(messageBuff, sizeof(messageBuff), "Thread with id %d was \"freed\"\n", inThreadStruct->threadId);
	Log::LogPrint(messageBuff);
	
	TerminateThread(inThreadStruct->hThread, 0);
	inThreadStruct->isFree = TRUE;

	return 0;
}

void Thread::RunTask(LPTHREAD_START_ROUTINE Method)
{
	char messageBuff[100];
	snprintf(messageBuff, sizeof(messageBuff), "Thread with id %d was \"locked\"\n", inThread.threadId);
	Log::LogPrint(messageBuff);
	CreateWorkerThread(Method);
	HANDLE tempThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadProc, &inThread, NULL, NULL);
}

void Thread::CreateWorkerThread(LPTHREAD_START_ROUTINE Method)
{
	SetFreeState(FALSE);
	char messageBuff[256];
	snprintf(messageBuff, sizeof(messageBuff), "Thread with id %d has been started execute method \"%s\"\n", inThread.threadId, inThread.methodName);
	Log::LogPrint(messageBuff);
	inThread.hThread = CreateThread(NULL, NULL, Method, NULL, 0, NULL);
}

void Thread::SetFreeState(BOOL newState)
{
	inThread.isFree = FALSE;
}

BOOL Thread::IsFree()
{
	return inThread.isFree;
}