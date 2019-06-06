
//
// GUMemory.cpp
//

#include <stdafx.h>
#include <GUMemory.h>
#include <iostream>


using namespace std;


//
// Memory allocation / free counters
//

static unsigned long			total_malloc_calls = 0;
static unsigned long			total_free_calls = 0;



//
// Memory handling functions
//

void gu_memAssertFail(const char *ptrString, const char *fnString) {

	cout << "gu_memAssert(" << ptrString << ") in function " << fnString << "() failed.  Aborting process.\n\n";
	abort();
}




//
// Memory tracking functions
//


void *gu_malloc(size_t memreq)
{
	void			*ptr;

#ifdef __GU_DEBUG_MEMORY__
	// Avoid recursive call due to malloc = gu_malloc
#undef malloc

	ptr = malloc(memreq);

#define malloc gu_malloc

#else

	ptr = malloc(memreq);

#endif


	if (ptr)
		total_malloc_calls++;

	return ptr;
}


void *gu_calloc(size_t num, size_t size)
{
	void			*ptr;

#ifdef __GU_DEBUG_MEMORY__
	// Avoid recursive call due to calloc = gu_calloc
#undef calloc

	ptr = calloc(num, size);

#define calloc gu_calloc

#else

	ptr = calloc(num, size);

#endif


	if (ptr)
		total_malloc_calls++;

	return ptr;
}


void* gu_aligned_malloc(size_t _Size, size_t _Alignment)
{
	void			*ptr;

#ifdef __GU_DEBUG_MEMORY__
	// Avoid recursive call due to _aligned_malloc = gu_aligned_malloc
#undef _aligned_malloc

	ptr = _aligned_malloc(_Size, _Alignment);

#define _aligned_malloc		gu_aligned_malloc

#else

	ptr = _aligned_malloc(_Size, _Alignment);

#endif

	if (ptr)
		total_malloc_calls++;

	return ptr;
}


void gu_free(void *ptr)
{
	if (ptr)
		total_free_calls++;

#ifdef __GU_DEBUG_MEMORY__
	// Avoid recursive call due to free = gu_free
#undef free

	free(ptr);

#define	free gu_free

#else

	free(ptr);

#endif

}


void gu_aligned_free(void* ptr)
{
	if (ptr)
		total_free_calls++;

#ifdef __GU_DEBUG_MEMORY__
	// Avoid recursive call due to _aligned_free = gu_aligned_free
#undef _aligned_free

	_aligned_free(ptr);

#define _aligned_free gu_aligned_free

#else

	_aligned_free(ptr);

#endif

}



#ifdef __GU_DEBUG_MEMORY__

void *operator new(size_t size){

	return malloc(size);
}

void operator delete(void *ptr) {

	free(ptr);
}


void *operator new[](size_t size) {

	return malloc(size);
}


void operator delete[](void *ptr) {

	free(ptr);
}

#endif



//
// Memory reporting functions
//

unsigned long gu_memory_allocations() {

	return total_malloc_calls;
}


unsigned long gu_memory_deallocations() {

	return total_free_calls;
}


unsigned long gu_memory_error() {

	return gu_memory_allocations() - gu_memory_deallocations();
}


void gu_memory_report() {

	cout << "malloc(" << gu_memory_allocations() << ") free(" << gu_memory_deallocations() << ") error(" << gu_memory_error() << ")\n\n";
}



/*

Memory counter adjustment functions

We can broadly classify 4 different types of memory allocation that will affect the application of this library to accurately track memory leaks...

(i) Appication managed allocation and disposal of memory resources.  These are trapped by this library and do not present a problem.

(ii) OS allocated memory.  The Operating system or pre-compiled API will allocate memory and return it to the application.  The application has "ownership" of the returned memory and is responsible for disposal.  This can affect the alloc/dealloc counters maintained by this library.  To address this the compensate_malloc_count() function can be used explicitly.  This increments the alloc counter to reflect the OS memory allocations returned to the application.  When the application then frees the memory, the alloc/dealloc counters should correspond.

(iii) Application allocated memory - The application allocates memory which is then passed to the OS / API.  For example, the GLU library implements callbacks that return memory to the GLU API.  Ownership of the memory is passed to the API.  To address this the compensate_free_count() function can be called to increment the dealloc counter to reflect the memory that the OS / API will free up.

(iv) Static allocations during optimisation.  When building against existing libraries, different malloc/dealloc behaviour can be observed for code compiled with the debug and release versions of the libraries.  For example, certain libraries may allocate buffers so memeory allocation is not required on subsequent calls.

*/


// Compensate_malloc_count: Some memory may be allocated by the operating system not not explicity with malloc by the application.  In such cases, freeing the memory explicity can give rise to malloc counter errors.  This should be called after freeing os allocated memory to ensure the counter is correct.
void compensate_malloc_count(unsigned long c)
{
	total_malloc_calls += c;
}


// Compensate_free_count: Memory may be allocated by the application and returned to the OS / calling API.  It may appear a leak has occured  when in fact the OS has freed the memory.  Use this to increment the number of deallocs to refelect memory returned to the OS.  This should be called after or just before returning memory to the OS to ensure the counter is correct.
void compensate_free_count(unsigned long c)
{
	total_free_calls += c;
}
