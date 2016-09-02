#include <Windows.h>

#ifndef CIRCBUFFER_H
#define CIRCBUFFER_H

class CircBufferFixed
{
private:
	// your private stuff,
	// implementation details, etc.
	//
	struct Header
	{
		size_t id;
		size_t length;
		size_t padding;
		// maybe number of consumers here?
	};

public:
	// Constructor
	CircBufferFixed(
		LPCWSTR buffName,          // unique name
		const size_t& buffSize,    // size of the whole filemap
		const bool& isProducer,    // is this buffer going to be used as producer
		const size_t& chunkSize);  // round up messages to multiple of this.
	// Destructor
	~CircBufferFixed();

	size_t canRead();  
	size_t canWrite(); 
					   
	bool push(const void* msg, size_t length);
	bool pop(char* msg, size_t& length);
};

#endif