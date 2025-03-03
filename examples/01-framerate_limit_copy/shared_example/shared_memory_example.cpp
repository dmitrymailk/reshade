#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>

using namespace boost::interprocess;

// Define an allocator for shared memory
typedef allocator<int, managed_shared_memory::segment_manager> ShmemAllocator;

// Define a shared memory vector
typedef vector<int, ShmemAllocator> MySharedVector;

// Define a shared memory map (associative array)
typedef std::pair<const int, int> MyMapValueType;
typedef map<int, int, std::less<int>, ShmemAllocator> MySharedMap;


int main() {
	// Define the shared memory name
	const char *shared_memory_name = "MySharedMemory";

	// **Create or open the shared memory segment**

	// **Cleanup previous shared memory (Optional - but good practice)**
	shared_memory_object::remove(shared_memory_name);


	try {
		// **1. Create the shared memory segment**
		managed_shared_memory segment(create_only, shared_memory_name, 65536); // 64KB

		// **2. Construct objects in the shared memory**

		// Allocate a shared memory allocator.
		ShmemAllocator alloc_inst(segment.get_segment_manager());

		// Construct a shared memory vector.
		MySharedVector *myvector = segment.construct<MySharedVector>("myvector")(alloc_inst);
		// equivalent to:
		// MySharedVector* myvector = new (segment.allocate(sizeof(MySharedVector))) MySharedVector(alloc_inst);


		// Construct a shared memory map
		MySharedMap *mymap = segment.construct<MySharedMap>("mymap")(std::less<int>(), alloc_inst);
		// equivalent to:
		// MySharedMap* mymap = new (segment.allocate(sizeof(MySharedMap))) MySharedMap(std::less<int>(), alloc_inst);


		// **3. Populate the shared memory objects**
		for (int i = 0; i < 10; ++i) {
			myvector->push_back(i);
			mymap->insert(MyMapValueType(i, i * 2));
		}

		std::cout << "Shared memory created and populated. Process ID: " << getpid() << std::endl;
		std::cout << "Vector contents: ";
		for (int i = 0; i < myvector->size(); ++i) {
			std::cout << (*myvector)[i] << " ";
		}
		std::cout << std::endl;

		std::cout << "Map contents: ";
		for (const auto &pair : *mymap) {
			std::cout << "(" << pair.first << ", " << pair.second << ") ";
		}
		std::cout << std::endl;

		// Optionally, wait for user input to keep the shared memory alive
		std::cout << "Press Enter to release shared memory and exit." << std::endl;
		std::cin.get();

		// **4. Destroy the shared memory objects (optional but good practice)**
		segment.destroy<MySharedVector>("myvector");
		segment.destroy<MySharedMap>("mymap");


		// **5. Remove the shared memory segment (optional, but recommended)**
		shared_memory_object::remove(shared_memory_name);


	}
	catch (const interprocess_exception &e) {
		std::cerr << "Interprocess exception caught: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
