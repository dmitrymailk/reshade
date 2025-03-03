Combine this programs into one. I want to 
1. Write data into shared memory
2. change change python semaphore on python side
3. read and change shared memory
4. trigger c++ semaphore on python side (this is indicator that i done task) 
5. repeat this in simple loop 10 times
```cpp
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
```
```python
import multiprocessing.shared_memory
import numpy as np
import win32event  # PyWin32 library for Windows events/semaphores
import pywintypes  # Part of PyWin32
import win32api

# Constants (MUST MATCH C++ CODE)
SHARED_MEMORY_NAME = "my_int_array"
SEMAPHORE_NAME = "my_semaphore"  # Match the C++ name
ARRAY_SIZE = 100
SHARED_MEMORY_SIZE = ARRAY_SIZE * 4

try:
    # --- Attach to the shared memory ---
    shm = multiprocessing.shared_memory.SharedMemory(name=SHARED_MEMORY_NAME, create=False, size=SHARED_MEMORY_SIZE)
    int_array = np.ndarray((ARRAY_SIZE,), dtype=np.int32, buffer=shm.buf)

    # --- Open the semaphore ---
    try:
        hSemaphore = win32event.OpenSemaphore(win32event.SYNCHRONIZE | win32event.EVENT_MODIFY_STATE, False, SEMAPHORE_NAME,)
    except pywintypes.error as e:
        print(f"Error opening semaphore: {e}")
        hSemaphore = None
        raise  # Re-raise the exception to be caught in the outer try block


    if hSemaphore is None:
        print("Failed to open semaphore.")
        raise Exception("Semaphore open failed")

    # --- Modify the array ---
    print("Modifying the array...")
    for i in range(ARRAY_SIZE):
        int_array[i] = int_array[i] + 5

    print("Array modified. Signaling completion...")
    win32event.WaitForSingleObject()
    # --- Signal the semaphore ---
    win32event.ReleaseSemaphore(hSemaphore, 1)  # Increment the semaphore count

    print("Semaphore signaled.")

except FileNotFoundError:
    print("Shared memory not found. Run the C++ program first.")
except Exception as e:
    print(f"An error occurred: {e}")
```
```cpp
#include <iostream>
#include <windows.h>
#include <string>
#include <thread>
#include <chrono>

int main() {
	const wchar_t *semaphoreName = L"MyPythonSemaphore";
	HANDLE semaphoreHandle;
	LONG releaseCount = 1;
	LONG previousCount;

	// Open the existing named semaphore
	semaphoreHandle = OpenSemaphoreW(
		SEMAPHORE_ALL_ACCESS, // Desired Access
		FALSE,                // bInheritHandle
		semaphoreName       // lpName
	);

	if (semaphoreHandle == NULL) {
		std::cerr << "OpenSemaphore failed: " << GetLastError() << std::endl;
		return 1;
	}

	std::cout << "Semaphore opened in C++.  Press Enter to trigger the Python side." << std::endl;

	for (int i = 0; i < 5; ++i) { // Trigger 5 times
		// std::cout << "Press Enter to release semaphore (iteration " << i + 1 << ")..." << std::endl;
		// std::cin.get(); // Wait for input

		// Release the semaphore
		if (!ReleaseSemaphore(semaphoreHandle, releaseCount, &previousCount)) {
			std::cerr << "ReleaseSemaphore failed: " << GetLastError() << std::endl;
			CloseHandle(semaphoreHandle);
			return 1;
		}

		std::cout << "Semaphore released." << std::endl;
		// std::this_thread::sleep_for(std::chrono::milliseconds(500));  // avoid spam
	}

	// Clean up
	CloseHandle(semaphoreHandle);
	std::cout << "Semaphore handle closed." << std::endl;

	return 0;
}
```
```python
import win32event
import win32con
import time
import os

semaphore_name = "MyPythonSemaphore" # Must be consistent

# Create the named semaphore
semaphore_handle = win32event.CreateSemaphore(None, 0, 2, semaphore_name)
# None : Security Attributes, 0: Initial Count , 2: Maximum count , "Semaphor Name"

if semaphore_handle is None:
    print(f"Error creating semaphore: {win32event.GetLastError()}")
    exit(1)

print("Semaphore created in Python. Waiting for signals...")

try:
    while True:
        # Wait for the semaphore to be signaled
        result = win32event.WaitForSingleObject(semaphore_handle, win32event.INFINITE)

        if result == win32con.WAIT_OBJECT_0:
            print("Semaphore signaled! Continuing...")
            # Do something after the semaphore is released
            print("Work done. Waiting again...")  # Signal ready
        elif result == win32con.WAIT_ABANDONED:
            print("Semaphore abandoned! Exiting...")
            break  # Exit loop if abandoned
        else:
            print(f"Wait failed: {result}")
            break  # Exit loop

except KeyboardInterrupt:
    print("Interrupted by user. Exiting...")

finally:
    # Clean up
    win32event.CloseHandle(semaphore_handle)
    print("Semaphore handle closed.")

```