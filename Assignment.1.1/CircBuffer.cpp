#include "CircBuffer.h"

CircularBuffer::CircularBuffer(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
	messageData = new char[buffSize];
	controlData = new size_t[4];

	head = controlData;
	tail = head + 1;
	clients = tail + 1;
	freeMemory = clients + 1;

	*head = 0;
	*tail = 0;
	//if (/*first*/)
	//{
	//	*clients = 0;
	//}
	if (isProducer == false)
	{
		*clients += 1;
	}
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
		memcpy(messageData + *head + sizeof(Header), msg, messageSize);
		*freeMemory -= messageSize;
		*head = (*head + messageSize) % bufferSize;
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
		size_t messageSize = sizeof(Header) + h->length + h->padding;
		memcpy(msg, &messageData[*tail + sizeof(Header)], messageSize);
		*freeMemory += h->length + sizeof(Header) + h->padding;
		*tail = (*tail + h->length + sizeof(Header) + h->padding) % bufferSize;
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
