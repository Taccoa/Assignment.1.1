#include "CircBuffer.h"

CircularBuffer::CircularBuffer(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
	messageData = new char[buffSize];
	controlData = new size_t[3];

	head = controlData;
	tail = head + 1;
	freeMemory = tail + 1;

	*head = 0;
	*tail = 0;
	*freeMemory = buffSize;

	bufferSize = buffSize;
	this->chunkSize = chunkSize;
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
		size_t message_head = length + sizeof(Header);
		size_t remaining = message_head % chunkSize;
		size_t padding = chunkSize - remaining;
		size_t messageSize = sizeof(Header) + length + padding;

		Header header{ msgID++, length, padding};
		memcpy(messageData + *head, &header, sizeof(Header));
		memcpy(messageData + *head + sizeof(Header), msg, length);
		*freeMemory -= length + sizeof(Header);
		*head = (*head + length + sizeof(Header)) % bufferSize;
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
		Header* h = (Header*)(&messageData[*tail]);
		length = h->length;
		memcpy(msg, &messageData[*tail + sizeof(Header)], length);
		*freeMemory += h->length + sizeof(Header);
		*tail = (*tail + h->length + sizeof(Header)) % bufferSize;
		return true;
	}
	else
	{
		return false;
	}
}

size_t CircularBuffer::canRead()
{
	// returns how many bytes are available for reading.
	return size_t();
}

size_t CircularBuffer::canWrite()
{
	// returns how many bytes are free in the buffer.
	return size_t();
}
