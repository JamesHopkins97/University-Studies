
//
// GUObject.h
//

// Base class incorporating a reference counting retain-release mechanism

#pragma once

#include <GUMemory.h>

// Define base abstract class
class GUObject {

private:
	unsigned int			retainCount;

public:
	GUObject();
	// Important - all derived classed have virtual destructors
	// This ensures appropriate destructor called for based-class pointer referenced sub-classes
	virtual ~GUObject();

	void retain();
	bool release();
	virtual void report();

	// Accessor methods
	unsigned int getRetainCount();
};
