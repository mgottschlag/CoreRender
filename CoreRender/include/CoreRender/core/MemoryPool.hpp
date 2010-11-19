/*
Copyright (C) 2010, Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _CORERENDER_CORE_MEMORYPOOL_HPP_INCLUDED_
#define _CORERENDER_CORE_MEMORYPOOL_HPP_INCLUDED_

#include <vector>
#include <tbb/spin_mutex.h>

// Use this to replace the garbage collector with normal malloc()/free() pairs
// for every allocation to make memory debuggers like valgrind work properly
//#define CR_MEMORY_DEBUGGING

#ifdef CR_MEMORY_DEBUGGING
#include <cstdlib>
struct MallocEntry
{
	MallocEntry *next;
};
#endif

namespace cr
{
namespace core
{

	/**
	 * Memory pool class using simple garbage collection. This class can be used
	 * if the life time of many dynamically allocated objects is known to end
	 * at a certain point in time, for example all render batches are destroyed
	 * at the end of the frame. This class allocates all these from larger areas
	 * of memory to reduce the number of calls to malloc/free and can easily
	 * reuse the memory after reset() has been called and all objects have been
	 * deleted. allocate() however is not viable for very large objects, these
	 * should be allocated in a different way.
	 *
	 * Note that this class only works with POD types which do not have any
	 * constructor or destructor as the latter is not called when the memory
	 * is deleted again.
	 *
	 * The memory allocated by this class is divided into large pages, when one
	 * is full, the next one is allocated and used for many calls to allocate().
	 */
	class MemoryPool
	{
		public:
			/**
			 * Constructor.
			 * @param pagesize Size of one page allocated by the class. Should
			 * be much larger than the objects which are going to be allocated.
			 */
			MemoryPool(unsigned int pagesize = 1048576)
				: used(0), firstdestructor(0), lastdestructor(0)
			{
				// Round up page size to 4k pages
				pagesize = (pagesize + 0xFFF) & ~0xFFF;
				this->pagesize = pagesize;
				// Allocate a single page
				currentmemory = allocPage(pagesize);

#ifdef CR_MEMORY_DEBUGGING
				mallocmem = 0;
#endif
			}
			/**
			 * Destructor.
			 */
			~MemoryPool()
			{
				// Call destructors
				callDestructors();
				// Free memory
				freePage(currentmemory, pagesize);
				for (unsigned int i = 0; i < usedmemory.size(); i++)
					freePage(usedmemory[i], pagesize);
				for (unsigned int i = 0; i < freememory.size(); i++)
					freePage(freememory[i], pagesize);
#ifdef CR_MEMORY_DEBUGGING
				MallocEntry *mallocentry = mallocmem;
				while (mallocentry)
				{
					MallocEntry *next = mallocentry->next;
					free(mallocentry);
					mallocentry = next;
				}
				mallocmem = 0;
#endif
			}

			/**
			 * Frees the memory returned by all previous calls to allocate().
			 * This function also deletes all pages which are currently not in
			 * use to reduce memory usage of the program and to adapt to
			 * changing needs of the program. It however leaves all pages
			 * allocated which have been used so that they do not have to be
			 * reallocated at once.
			 */
			void reset()
			{
				// Call destructors
				callDestructors();
				// Delete unused free pages
				for (unsigned int i = 0; i < freememory.size(); i++)
					freePage(freememory[i], pagesize);
				freememory.clear();
				// Move used pages to the free page list
				freememory = usedmemory;
				usedmemory.clear();
				// Start at the current page from the beginning
				used = 0;
#ifdef CR_MEMORY_DEBUGGING
				MallocEntry *mallocentry = mallocmem;
				while (mallocentry)
				{
					MallocEntry *next = mallocentry->next;
					free(mallocentry);
					mallocentry = next;
				}
				mallocmem = 0;
#endif
			}
			/**
			 * Frees the memory returned by all previous calls to allocate().
			 * Different to reset(), this function allocates as many pages as
			 * are needed to allocate size bytes, reusing existing pages if
			 * possible.
			 * @param size Amount of memory initially allocated.
			 */
			void reset(unsigned int size)
			{
				unsigned int pagecount = (size + pagesize - 1) / pagesize;
				// Call destructors
				callDestructors();
				// We have one page in currentmemory
				pagecount -= 1;
				// Allocate enough pages, reusing existing pages
				if (freememory.size() >= pagecount)
				{
					for (unsigned int i = 0; i < usedmemory.size(); i++)
						freePage(usedmemory[i], pagesize);
					usedmemory.clear();
					for (unsigned int i = pagecount; i < freememory.size(); i++)
						freePage(freememory[i], pagesize);
					freememory.resize(pagecount);
				}
				else if (freememory.size() + usedmemory.size() >= pagecount)
				{
					for (unsigned int i = 0; i < pagecount - freememory.size(); i++)
						freememory.push_back(usedmemory[i]);
					for (unsigned int i = pagecount - freememory.size();
						i < usedmemory.size(); i++)
						freePage(usedmemory[i], pagesize);
					usedmemory.clear();
				}
				else
				{
					unsigned int freepages = freememory.size();
					freememory.resize(pagecount);
					for (unsigned int i = 0; i < usedmemory.size(); i++)
						freememory[freepages + i] = usedmemory[i];
					freepages += usedmemory.size();
					usedmemory.clear();
					for (unsigned int i = freepages; i < pagecount; i++)
						freememory[i] = allocPage(pagesize);
				}
				// Reset current memory page
				used = 0;
#ifdef CR_MEMORY_DEBUGGING
				MallocEntry *mallocentry = mallocmem;
				while (mallocentry)
				{
					MallocEntry *next = mallocentry->next;
					free(mallocentry);
					mallocentry = next;
				}
				mallocmem = 0;
#endif
			}
			/**
			 * Frees the memory returned by all previous calls to allocate() and
			 * reallocates all memory pages with a new page size.
			 * @param size Amount of memory initially allocated.
			 * @param pagesize Size of one memory page.
			 */
			void reset(unsigned int size, unsigned int pagesize)
			{
				// Round up page size to 4k pages
				pagesize = (pagesize + 0xFFF) & ~0xFFF;
				// Call destructors
				callDestructors();
				// Free all existing pages
				freePage(currentmemory, this->pagesize);
				for (unsigned int i = 0; i < usedmemory.size(); i++)
					freePage(usedmemory[i], this->pagesize);
				for (unsigned int i = 0; i < freememory.size(); i++)
					freePage(freememory[i], this->pagesize);
				usedmemory.clear();
				// Set new page size
				this->pagesize = pagesize;
				// Allocate new pages
				unsigned int pagecount = (size + pagesize - 1) / pagesize;
				currentmemory = allocPage(pagesize);
				freememory.resize(pagecount - 1);
				for (unsigned int i = 0; i < pagecount - 1; i++)
					freememory[i] = allocPage(pagesize);
				// Reset current memory page
				used = 0;
#ifdef CR_MEMORY_DEBUGGING
				MallocEntry *mallocentry = mallocmem;
				while (mallocentry)
				{
					MallocEntry *next = mallocentry->next;
					free(mallocentry);
					mallocentry = next;
				}
				mallocmem = 0;
#endif
			}

			/**
			 * Allocates a certain amount of memory. The pointer which is
			 * returned is only valid until reset() is called.
			 * @param size Size of the memory allocated (in bytes).
			 * @return Pointer to the allocated memory.
			 */
			void *allocate(unsigned int size)
			{
#ifndef CR_MEMORY_DEBUGGING
				// TODO: Handle large allocations properly?
				tbb::spin_mutex::scoped_lock lock(mutex);
				if (used + size <= pagesize)
				{
					// We have enough memory on this page
					void *memory = (char*)currentmemory + used;
					used += size;
					return memory;
				}
				else if (freememory.empty())
				{
					// Allocate a new page
					usedmemory.push_back(currentmemory);
					currentmemory = allocPage(pagesize);
					used = size;
					return currentmemory;
				}
				else
				{
					// Start a new page
					usedmemory.push_back(currentmemory);
					currentmemory = freememory.back();
					freememory.pop_back();
					used = size;
					return currentmemory;
				}
#else
				tbb::spin_mutex::scoped_lock lock(mutex);
				void *ptr = malloc(size + sizeof(MallocEntry));
				MallocEntry *mallocentry = (MallocEntry*)ptr;
				if (!mallocmem)
					mallocentry->next = 0;
				else
					mallocentry->next = mallocmem;
				mallocmem = mallocentry;
				return (char*)ptr + sizeof(MallocEntry);
#endif
			}
		private:
			struct DestructorEntry
			{
				void (*callDestructor)(void*);
				void *ptr;
				DestructorEntry *next;
			};
			template<class T> static void callDestructor(void *ptr)
			{
				T *obj = (T*)ptr;
				obj->~T();
			}
		public:
			/**
			 * Allocates memory to place a single object in. Optionally the
			 * memory pool then will call the destructor for the object when the
			 * memory is released. Letting the engine call the destructor comes
			 * at slight memory costs as a separate list with the destructors
			 * has to be maintained. The user always has to call the constructor
			 * manually.
			 * @param calldestructor If true, an entry in the destructor list is
			 * created so that the destructor is called when the memory is
			 * released.
			 * @return Pointer to the allocated memory.
			 */
			template<class T> void *allocate(bool calldestructor = true)
			{
				// If no destructor call is wanted, just allocate the memory
				if (!calldestructor)
				{
					return allocate(sizeof(T));
				}
				// If a destructor call is wanted, allocate an extra destructor
				// list entry
				void *ptr = allocate(sizeof(DestructorEntry) + sizeof(T));
				DestructorEntry *destructor = (DestructorEntry*)ptr;
				destructor->callDestructor = callDestructor<T>;
				destructor->ptr = (char*)ptr + sizeof(DestructorEntry);
				destructor->next = 0;
				// Insert destructor into the destructor list
				tbb::spin_mutex::scoped_lock lock(mutex);
				if (!lastdestructor)
				{
					firstdestructor = destructor;
					lastdestructor = destructor;
				}
				else
				{
					lastdestructor->next = destructor;
					lastdestructor = destructor;
				}
				// Return memory
				return (char*)ptr + sizeof(DestructorEntry);
			}

			template<class T> void registerDestructor(T *object)
			{
				// Allocate an extra destructor list entry
				void *ptr = allocate(sizeof(DestructorEntry));
				DestructorEntry *destructor = (DestructorEntry*)ptr;
				destructor->callDestructor = callDestructor<T>;
				destructor->ptr = object;
				destructor->next = 0;
				// Insert destructor into the destructor list
				tbb::spin_mutex::scoped_lock lock(mutex);
				if (!lastdestructor)
				{
					firstdestructor = destructor;
					lastdestructor = destructor;
				}
				else
				{
					lastdestructor->next = destructor;
					lastdestructor = destructor;
				}
			}
		private:
			static void *allocPage(unsigned int size);
			static void freePage(void *page, unsigned int size);

			void callDestructors()
			{
				DestructorEntry *destructor = firstdestructor;
				// Call all destructors
				while (destructor)
				{
					destructor->callDestructor(destructor->ptr);
					destructor = destructor->next;
				}
				// Empty destructor list
				firstdestructor = 0;
				lastdestructor = 0;
			}

			unsigned int pagesize;

			void *currentmemory;
			unsigned int used;

			DestructorEntry *firstdestructor;
			DestructorEntry *lastdestructor;

			std::vector<void*> usedmemory;
			std::vector<void*> freememory;

			tbb::spin_mutex mutex;

#ifdef CR_MEMORY_DEBUGGING
			MallocEntry *mallocmem;
#endif
	};
}
}

#endif
