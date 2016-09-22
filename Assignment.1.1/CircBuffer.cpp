#include "CircBuffer.h"
#include <conio.h>
#include <tchar.h>
#include <stdio.h>

class Mutex {
private:
	HANDLE handle;
public:
	Mutex(LPCWSTR name)
	{
		handle = CreateMutex(nullptr, false, name);
	}
	~Mutex()
	{
		ReleaseMutex(handle);
	}
	void Lock(DWORD milliseconds = INFINITE)
	{
		WaitForSingleObject(handle, milliseconds);
	}
	void Unlock()
	{
		ReleaseMutex(handle);
	}
};

CircularBuffer::CircularBuffer(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
	control = new size_t[4];

	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, buffSize, buffName);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"), GetLastError());
	}

	controlFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(control), L"control");
	if (controlFileMap == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"), GetLastError());
	}

	messageData = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, buffSize);
	if (messageData == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
		CloseHandle(hMapFile);
	}

	controlData = (size_t*)MapViewOfFile(controlFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(control));
	if (controlData == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
		CloseHandle(controlFileMap);
	}

	head = controlData;
	tail = head + 1;
	clients = tail + 1;
	freeMemory = clients + 1;

	if (GetLastError() != ERROR_ALREADY_EXISTS)
	{
		*head = 0;
		*tail = 0;
		*clients = 0;
		*freeMemory = buffSize;
	}
	
	if (isProducer == false)
	{
		Mutex myMutex(L"myMutex");
		myMutex.Lock();
		*clients += 1;
		myMutex.Unlock();
		inTail = 0;
	}

	this->chunkSize = chunkSize;
	bufferSize = buffSize;
	msgID = 0;
}

CircularBuffer::~CircularBuffer()
{
	CloseHandle(hMapFile);
	CloseHandle(controlFileMap);
	UnmapViewOfFile(messageData);
	UnmapViewOfFile(controlData);
	delete[] control;
}

void createDummy(char *s, const int lenght)
{
	static const char alphanumber[] =
		"0";

	for (auto i = 0; i < lenght; i++)
	{
		s[i] = alphanumber[rand() % (sizeof(alphanumber) - 1)];
	}
	s[lenght - 1] = 0;
}

bool CircularBuffer::push(const void * msg, size_t length)
{
	size_t message_head = length + sizeof(Header);
	size_t remaining = message_head % chunkSize;
	size_t padding = chunkSize - remaining;
	size_t messageSize = sizeof(Header) + length + padding;

	if (messageSize < (*freeMemory - 1))
	{
		Header header{ msgID++, length, padding, *clients };
		memcpy(messageData + *head, &header, sizeof(Header));
		memcpy(messageData + *head + sizeof(Header), msg, messageSize);
		Mutex myMutex(L"myMutex");
		myMutex.Lock();
		*freeMemory -= messageSize;
		*head = (*head + messageSize) % bufferSize;
		myMutex.Unlock();
		return true;
	}
	else
	{
		return false;
	}
}

bool CircularBuffer::pop(char * msg, size_t & length)
{
	if (*freeMemory < bufferSize)
	{
		if (*head != inTail)
		{
			Header* h = (Header*)(&messageData[*tail]);
			length = h->length;
			size_t messageSize = sizeof(Header) + h->length + h->padding;
			memcpy(msg, &messageData[inTail + sizeof(Header)], messageSize);
			inTail = (inTail + h->length + sizeof(Header) + h->padding) % bufferSize;
			Mutex myMutex(L"myMutex");
			myMutex.Lock();
			h->consumersLeft -= 1;
			if (h->consumersLeft == 0)
			{
				*freeMemory += h->length + sizeof(Header) + h->padding;
				*tail = inTail;
			}
			myMutex.Unlock();
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}