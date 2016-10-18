#pragma once
#include <Windows.h>
#include <process.h>
#include <iostream>

class ThreadPool
{
public:
	void CreateTask(LPTHREAD_START_ROUTINE);
	ThreadPool();
	~ThreadPool();

	static DWORD freeThreadCount;
};

