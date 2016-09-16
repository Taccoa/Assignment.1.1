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
	messageData = new char[buffSize];
	controlData = new size_t[4];

	head = controlData;
	tail = head + 1;
	clients = tail + 1;
	freeMemory = clients + 1;

	/*TCHAR szMsg[] = TEXT("Message from first process.");*/

	HANDLE hMapFile;
	LPCTSTR pBuf;

	HANDLE controlFileMap;
	LPCTSTR controlpBuf;

	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, buffSize, buffName);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"), GetLastError());
	}
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, buffName);
		if (hMapFile == NULL)
		{
			_tprintf(TEXT("Could not open file mapping object (%d).\n"), GetLastError());
		}
	}

	controlFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(controlData), L"control");
	if (controlFileMap == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"), GetLastError());
	}
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		controlFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"control");
		if (controlFileMap == NULL)
		{
			_tprintf(TEXT("Could not open file mapping object (%d).\n"), GetLastError());
		}
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, buffSize);
	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
		CloseHandle(hMapFile);
	}

	controlpBuf = (LPTSTR)MapViewOfFile(controlFileMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(controlData));
	if (controlpBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
		CloseHandle(controlFileMap);
	}

	/*CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
	_getch();*/

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);

	UnmapViewOfFile(controlpBuf);
	CloseHandle(controlFileMap);

	
	if (isProducer == true)
	{
		*head = 0;
		*tail = 0;
		*clients = 0;
		*freeMemory = buffSize;
	}
	if (isProducer == false)
	{
		*clients += 1;
	}

	this->chunkSize = chunkSize;
	bufferSize = buffSize;
	msgID = 0;
}

CircularBuffer::~CircularBuffer()
{
	delete[] messageData;
	delete[] controlData;
}

bool CircularBuffer::push(const void * msg, size_t length)
{
	if (length < (*freeMemory - 1))
	{
		Mutex myMutex(L"myMutex");
		myMutex.Lock();
		size_t message_head = length + sizeof(Header);
		size_t remaining = message_head % chunkSize;
		size_t padding = chunkSize - remaining;
		size_t messageSize = sizeof(Header) + length + padding;

		Header header{ msgID++, length, padding, *clients};
		memcpy(messageData + *head, &header, sizeof(Header));
		memcpy(messageData + *head + sizeof(Header), msg, messageSize);
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
		Mutex myMutex(L"myMutex");
		myMutex.Lock();
		Header* h = (Header*)(&messageData[*tail]);
		length = h->length;
		size_t messageSize = sizeof(Header) + h->length + h->padding;
		memcpy(msg, &messageData[*tail + sizeof(Header)], messageSize);
		h->consumersLeft -= 1;
		if (h->consumersLeft == 0)
		{
			*freeMemory += h->length + sizeof(Header) + h->padding;
			*tail = (*tail + h->length + sizeof(Header) + h->padding) % bufferSize;
		}
		myMutex.Unlock();
		return true;
	}
	else
	{
		return false;
	}
}