#include "MyThread.h"

void cool(int number)
{
	printf("Number is: %d\r\n", number);
}

int main()
{
	ThreadPool *threadPool = new ThreadPool();
	threadPool->CreateTask((LPTHREAD_START_ROUTINE)cool);
	threadPool->CreateTask((LPTHREAD_START_ROUTINE)cool);
}