
//
// GUObject.cpp
//

#include <stdafx.h>
#include <GUObject.h>
#include <iostream>

using namespace std;


GUObject::GUObject() {

	retainCount = 1; // Initialise retainCount - calling function adopts ownership
}


GUObject::~GUObject() {
}


// Retain object
void GUObject::retain() {

	retainCount++;
}

// Release ownership of object.  If retainCount=0 then delete object.  Return true if the object is deleted successfully - so calling function knows if pointer/handle to object is valid or not.
bool GUObject::release() {

	retainCount--;
	
	if (retainCount == 0) {

		delete(this);
		return true;
	}

	return false;
}


void GUObject::report() {

	cout << "retain count = " << retainCount << endl;
}


// Accessor methods

unsigned int GUObject::getRetainCount() {

	return retainCount;
}


