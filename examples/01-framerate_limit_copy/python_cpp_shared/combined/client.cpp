#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <chrono>

const int ARRAY_SIZE = 100;
const int SHARED_MEMORY_SIZE = ARRAY_SIZE * sizeof(int32_t);
const char *SHARED_MEMORY_NAME = "my_int_array";

int main() {
	const wchar_t *semaphoreName = L"PythonSemaphore";
	const char *cppSemaphoreName = "CppSemaphore";
	HANDLE pythonSemaphore;
	LONG releaseCount = 1;
	LONG previousCount;

	// Open the existing named semaphore
	pythonSemaphore = OpenSemaphoreW(
		SEMAPHORE_ALL_ACCESS, // Desired Access
		FALSE,                // bInheritHandle
		semaphoreName       // lpName
	);

	if (pythonSemaphore == NULL) {
		std::cerr << "OpenSemaphore failed: " << GetLastError() << std::endl;
		return 1;
	}

	HANDLE cppSemaphore = CreateSemaphore(
		NULL,     // default security attributes
		0,        // initial count: starts at 0 (blocked)
		1,        // maximum count: only allows one signal
		cppSemaphoreName);  // semaphore name

	if (cppSemaphore == NULL) {
		printf("Could not create semaphore (%lu).\n", GetLastError());
		return 1;
	}

	// --- Create or Open Shared Memory ---
	HANDLE hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_MEMORY_SIZE, SHARED_MEMORY_NAME);
	if (hMapFile == NULL) {
		printf("Could not create file mapping object (%lu).\n", GetLastError());
		return 1;
	}
	LPVOID lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEMORY_SIZE);
	if (lpMapAddress == NULL) {
		printf("Could not map view of file (%lu).\n", GetLastError());
		CloseHandle(hMapFile);
		return 1;
	}
	int32_t *intArray = (int32_t *)lpMapAddress;
	// --- Initialize the array ---
	for (int i = 0; i < ARRAY_SIZE; ++i) {
		intArray[i] = i * 2;
	}

	std::cout << "Semaphore opened in C++.  Press Enter to trigger the Python side." << std::endl;

	for (int i = 0; i < 5; ++i) { // Trigger 5 times
		// std::cout << "Press Enter to release semaphore (iteration " << i + 1 << ")..." << std::endl;
		// std::cin.get(); // Wait for input

		// Release the semaphore
		if (!ReleaseSemaphore(pythonSemaphore, releaseCount, &previousCount)) {
			std::cerr << "ReleaseSemaphore failed: " << GetLastError() << std::endl;
			CloseHandle(pythonSemaphore);
			return 1;
		}
		std::cout << "Python Semaphore released." << std::endl;
		WaitForSingleObject(cppSemaphore, INFINITE);
		for (int i = 0; i < ARRAY_SIZE; ++i) {
			std::cout << intArray[i] << " ";
		}
		std::cout << "Work Done." << std::endl;
		// std::this_thread::sleep_for(std::chrono::milliseconds(500));  // avoid spam
	}

	// Clean up
	UnmapViewOfFile(lpMapAddress);
	CloseHandle(hMapFile);
	CloseHandle(pythonSemaphore);
	std::cout << "Semaphore handle closed." << std::endl;

	return 0;
}
