#include <Windows.h>
#include <iostream>

using namespace std;

void Producer(size_t delay, size_t memorySize, size_t numMessages, size_t msgSize);
void Producer(size_t delay, size_t memorySize, size_t numMessages, size_t msgSize)
{
	cout << delay << endl;
}

void Consumer(size_t delay, size_t memorySize, size_t numMessages, size_t msgSize);
void Consumer(size_t delay, size_t memorySize, size_t numMessages, size_t msgSize)
{

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
		size_t MessageSize = atoi(argv[5]);

		if (argv[1] == "producer")
		{
			Producer(Delay, MemorySize, NumberOfMessages, MessageSize);
		}
		else if (argv[1] == "consumer")
		{
			Consumer(Delay, MemorySize, NumberOfMessages, MessageSize);
		}
	}

	cin.get();

	return 0;
}