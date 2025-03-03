#include <iostream>
#include <vector>
#include <windows.h>  // For Windows API
#include <cstdint>
#include <cstring>
#include <stdio.h>

const char *SHARED_MEMORY_NAME = "my_int_array";
const char *SEMAPHORE_NAME = "my_semaphore";  // Name for the semaphore
const int ARRAY_SIZE = 100;
const int SHARED_MEMORY_SIZE = ARRAY_SIZE * sizeof(int32_t);

int main() {
	HANDLE hMapFile, hSemaphore;
	LPVOID lpMapAddress;
	int32_t *intArray;

	// --- Create or Open Shared Memory ---
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_MEMORY_SIZE, SHARED_MEMORY_NAME);
	if (hMapFile == NULL) {
		printf("Could not create file mapping object (%lu).\n", GetLastError());
		return 1;
	}
	lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEMORY_SIZE);
	if (lpMapAddress == NULL) {
		printf("Could not map view of file (%lu).\n", GetLastError());
		CloseHandle(hMapFile);
		return 1;
	}
	intArray = (int32_t *)lpMapAddress;

	// --- Create or Open Semaphore ---
	hSemaphore = CreateSemaphore(
		NULL,     // default security attributes
		0,        // initial count: starts at 0 (blocked)
		1,        // maximum count: only allows one signal
		SEMAPHORE_NAME);  // semaphore name
	if (hSemaphore == NULL) {
		printf("Could not create semaphore (%lu).\n", GetLastError());
		UnmapViewOfFile(lpMapAddress);
		CloseHandle(hMapFile);
		return 1;
	}

	// --- Initialize the array ---
	for (int i = 0; i < ARRAY_SIZE; ++i) {
		intArray[i] = i * 2;
	}

	printf("Array initialized. Waiting for Python to complete...\n");

	// --- Wait for the semaphore (1st time) ---
	WaitForSingleObject(hSemaphore, INFINITE);  // Wait indefinitely
	printf("Python completed its task (1st time). Reading array:\n");

	// --- Read and print the modified array (1st time) ---
	for (int i = 0; i < ARRAY_SIZE; ++i) {
		std::cout << "intArray[" << i << "] = " << intArray[i] << std::endl;
	}

	// --- Set Semaphore Back to Zero & Wait Again ---
	printf("Resetting semaphore. Waiting for Python to complete again...\n");

	WaitForSingleObject(hSemaphore, INFINITE);


	// --- Read and print the modified array (2nd time) ---
	for (int i = 0; i < ARRAY_SIZE; ++i) {
		std::cout << "intArray[" << i << "] = " << intArray[i] << std::endl;
	}

	// --- Cleanup ---
	UnmapViewOfFile(lpMapAddress);
	CloseHandle(hMapFile);
	CloseHandle(hSemaphore); // Close the semaphore handle

	return 0;
}
