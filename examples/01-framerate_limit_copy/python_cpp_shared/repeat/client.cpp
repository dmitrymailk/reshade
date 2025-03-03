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
