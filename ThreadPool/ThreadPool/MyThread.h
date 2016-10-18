#pragma once
#include "ThreadPool.h"


class MyThread
{
public:
	HANDLE currentThread;

	void ExecuteMethod();
	void DestroyMyThread();
	MyThread(LPTHREAD_START_ROUTINE);
	~MyThread();
};