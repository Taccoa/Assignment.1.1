#include <Windows.h>
#include <iostream>
#include "CircBuffer.h"

using namespace std;

void Producer(DWORD delay, size_t memorySize, size_t numMessages, size_t msgSize);
void Consumer(DWORD delay, size_t memorySize, size_t numMessages, size_t msgSize);

void createRandom(char *s, const int lenght)
{
	static const char alphanumber[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (auto i = 0; i < lenght; i++)
	{
		s[i] = alphanumber[rand() % (sizeof(alphanumber) - 1)];
	}
	s[lenght-1] = 0;
}

// shared.exe producer|consumer delay memorySize numMessages random|msgSize
void usage()
{
	cout << "Usage: program [producer|consumer] delay memorySize numberMessages [random|msgSize]" << endl;
	cin.get();
	exit(1);
}

int main(int argc, char* argv[])
{
	if (argc < 6)
		usage();

	DWORD Delay = atoi(argv[2]);
	size_t MemorySize = atoi(argv[3]) * 1<<20;
	size_t NumberOfMessages = atoi(argv[4]);
	size_t MessageSize = atoi(argv[5]);

	if (strcmp(argv[1], "producer") == 0)
	{
		Producer(Delay, MemorySize, NumberOfMessages, MessageSize);
	}
	if (strcmp(argv[1], "consumer") == 0)
	{
		Consumer(Delay, MemorySize, NumberOfMessages, MessageSize);
	}
	
	return 0;
}

void Producer(DWORD delay, size_t memorySize, size_t numMessages, size_t msgSize)
{
	size_t maxMessageSize = memorySize / 4;
	size_t chunkSize = 256;

	CircularBuffer producer = CircularBuffer(L"Buffer", memorySize, true, chunkSize);

	char *buff = new char[maxMessageSize];
	int counter = 0;

	while (counter < numMessages)
	{
		if (delay != 0)
			Sleep(delay);

		size_t messageSize = msgSize;
		if (messageSize == 0)
		{
			messageSize = rand() * (maxMessageSize / RAND_MAX);
		}

		createRandom((char*)buff, messageSize);

		while (true)
		{
			if (producer.push(buff, messageSize))
			{
				counter++;
				cout << buff << endl;
				break;
			}
			else
				Sleep(1);
		}
	};
	Sleep(1000);
}

void Consumer(DWORD delay, size_t memorySize, size_t numMessages, size_t msgSize)
{
	size_t maxMessageSize = memorySize / 4;
	size_t chunkSize = 256;

	CircularBuffer consumer = CircularBuffer(L"Buffer", memorySize, false, chunkSize);

	char *msg = new char[maxMessageSize];
	int counter = 0;
	size_t len0 = 0;

	memset(msg, '\0', maxMessageSize);

	while (counter < numMessages)
	{
		if (delay > 0)
			Sleep(delay);

		if (consumer.pop(msg, len0))
		{
			counter++;
			cout << msg << endl;
		}
		else
		{
			Sleep(1);
		}
	}
	delete[] msg;
}