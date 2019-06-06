
//
// DXBlob.h
//

// Model a simple Binary Large Object (BLOB)

#pragma once

#include <iostream>
#include <cstdint>
#include <exception>
#include <GUObject.h>


class DXBlob {// : public GUObject {

	void			*buffer = nullptr;
	uint32_t		bufferSize = 0;

public:

	DXBlob(const uint32_t size) {

		try
		{
			buffer = malloc(size);

			if (!buffer)
				throw std::exception("DXBlob constructor: Cannot create buffer");

			bufferSize = size;
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;

			// Re-throw exception
			throw;
		}
	}

	~DXBlob() {

		if (buffer)
			free(buffer);
	}

	void *getBufferPointer() const {

		return buffer;
	}

	int32_t getBufferSize() const {

		return bufferSize;
	}
};
