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