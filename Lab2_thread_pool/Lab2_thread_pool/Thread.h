#include "Log.h"

#pragma once
class Thread
{
public:
	struct in
	{
		DWORD threadId;
		char* methodName;
		HANDLE hThread;
		BOOL isFree;
	} inThread;
	Thread(DWORD threadId);
	~Thread();
	static unsigned int __stdcall ThreadProc(in*);
	void RunTask(LPTHREAD_START_ROUTINE);
	void CreateWorkerThread(LPTHREAD_START_ROUTINE);
	BOOL IsFree();
	void SetFreeState(BOOL newState);
};

