#include "ThreadPool.h"

void TestMethod()
{
	Sleep(2000);
}

void SecondTestMethod()
{
	Sleep(3000);
}

int main()
{
	ThreadPool *threadPool = new ThreadPool(3);
	threadPool->InitializePool();

	threadPool->AssignTask((LPTHREAD_START_ROUTINE)TestMethod, "Test method");
	threadPool->AssignTask((LPTHREAD_START_ROUTINE)SecondTestMethod, "Second test method");
	threadPool->AssignTask((LPTHREAD_START_ROUTINE)SecondTestMethod, "Second test method");
	threadPool->AssignTask((LPTHREAD_START_ROUTINE)SecondTestMethod, "Second test method");

	threadPool->WaitAllThreads();
}