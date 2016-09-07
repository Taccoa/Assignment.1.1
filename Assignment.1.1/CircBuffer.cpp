#include "CircBuffer.h"

CircularBuffer::CircularBuffer(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
	if (isProducer == false)
	{

	}
	else if (isProducer == true)
	{

	}
}

CircularBuffer::~CircularBuffer(){}

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

bool CircularBuffer::push(const void * msg, size_t length)
{
	// try to send a message through the buffer,
	// if returns true then it succeeded, otherwise the message has not been sent.
	// it should return false if the buffer does not have enough space.
	return false;
}

bool CircularBuffer::pop(char * msg, size_t & length)
{
	// try to read a message from the buffer, and the implementation puts the content
	// in the memory. The memory is expected to be allocated by the program that calls
	// this function.
	return false;
}