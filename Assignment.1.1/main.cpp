#include <Windows.h>
#include <iostream>
#include "CircBuffer.h"

using namespace std;

void Producer(size_t delay, size_t memorySize, size_t numMessages, size_t msgSize);
void Producer(size_t delay, size_t memorySize, size_t numMessages, size_t msgSize)
{
	if (delay > 0)
		Sleep(delay);

	CircularBuffer(LPCWSTR("Buffer"), memorySize, true, msgSize);
}

void Consumer(size_t delay, size_t memorySize, size_t numMessages, size_t msgSize);
void Consumer(size_t delay, size_t memorySize, size_t numMessages, size_t msgSize)
{
	if (delay > 0)
		Sleep(delay);

	CircularBuffer(LPCWSTR("Buffer"), memorySize, false, msgSize);
}

// shared.exe producer|consumer delay memorySize numMessages random|msgSize

int main(int argc, char* argv[])
{
	HANDLE handleID;

	if (argc < 6)
	{
		cout << "Wrong number of aguments" << endl;
	}
	else
	{
		size_t Delay = atoi(argv[2]);
		size_t MemorySize = atoi(argv[3]);
		size_t NumberOfMessages = atoi(argv[4]);
		size_t MessageSize;
		if (strcmp(argv[5], "random"))
		{
			//RANDOM
		}
		else
		{
			MessageSize = atoi(argv[5]);
		}

		if (strcmp(argv[1], "producer") == 0)
		{
			Producer(Delay, MemorySize, NumberOfMessages, MessageSize);
		}
		else if (strcmp(argv[1], "consumer") == 0)
		{
			Consumer(Delay, MemorySize, NumberOfMessages, MessageSize);
		}
	}

	return 0;
}