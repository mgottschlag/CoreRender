
#include "CoreRender/core/MemoryPool.hpp"

#include <iostream>

using namespace cr;
using namespace core;

class TestClass
{
	public:
		TestClass()
		{
			testdata[0] = 0x0badc0de;
			testdata[1] = 0xdeadbeef;
			constructorcalls++;
		}
		~TestClass()
		{
			destructorcalls++;
			if (testdata[0] != 0x0badc0de)
				wrongdata++;
			else if (testdata[1] != 0xdeadbeef)
				wrongdata++;
		}

		static void getStats(unsigned int &constructorcalls,
		                     unsigned int &wrongdata,
		                     unsigned int &destructorcalls)
		{
			constructorcalls = TestClass::constructorcalls;
			wrongdata = TestClass::wrongdata;
			destructorcalls = TestClass::destructorcalls;
		}
	private:
		unsigned int testdata[2];
		static unsigned int constructorcalls;
		static unsigned int wrongdata;
		static unsigned int destructorcalls;
};

unsigned int TestClass::constructorcalls = 0;
unsigned int TestClass::wrongdata = 0;
unsigned int TestClass::destructorcalls = 0;

int main(int argc, char **argv)
{
	MemoryPool memory;
	for (unsigned int i = 0; i < 2; i++)
	{
		// Create a million instances of TestClass
		for (unsigned int j = 0; j < 1000000; j++)
		{
			void *ptr = memory.allocate<TestClass>();
			TestClass *obj = new(ptr) TestClass;
		}
		// Release memory again
		memory.reset();
	}
	// Check call numbers
	unsigned int constructorcalls;
	unsigned int wrongdata;
	unsigned int destructorcalls;
	TestClass::getStats(constructorcalls, wrongdata, destructorcalls);
	unsigned int errorcount = 0;
	if (constructorcalls != 2000000)
	{
		std::cout << "Constructor called " << constructorcalls
			<< " times, expected 2000000." << std::endl;
		errorcount++;
	}
	if (destructorcalls != 2000000)
	{
		std::cout << "Destructor called " << destructorcalls
			<< " times, expected 2000000." << std::endl;
		errorcount++;
	}
	if (wrongdata != 0)
	{
		std::cout << wrongdata << " objects were initialized with wrong values."
			<< std::endl;
		errorcount++;
	}
	std::cout << errorcount << " errors." << std::endl;
	return errorcount;
}
