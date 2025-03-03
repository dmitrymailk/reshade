import win32event
import win32con
import time
import os
import win32api
import multiprocessing.shared_memory
import numpy as np

PythonSemaphore = "PythonSemaphore" # Must be consistent
CppSemaphore = "CppSemaphore"
# Create the named semaphore
python_sem = win32event.CreateSemaphore(None, 0, 2, PythonSemaphore)
# None : Security Attributes, 0: Initial Count , 2: Maximum count , "Semaphor Name"
SHARED_MEMORY_NAME = "my_int_array"

ARRAY_SIZE = 100
SHARED_MEMORY_SIZE = ARRAY_SIZE * 4

if python_sem is None:
    print(f"Error creating semaphore: {win32event.GetLastError()}")
    exit(1)

print("Semaphore created in Python. Waiting for signals...")


while True:
    # Wait for the semaphore to be signaled
    result = win32event.WaitForSingleObject(python_sem, win32event.INFINITE)
    if result == win32con.WAIT_OBJECT_0:
        cpp_sem = win32event.OpenSemaphore(win32event.SYNCHRONIZE | win32event.EVENT_MODIFY_STATE, False, CppSemaphore,)
        # TODO: SOME WORK
        shm = multiprocessing.shared_memory.SharedMemory(name=SHARED_MEMORY_NAME, create=False, size=SHARED_MEMORY_SIZE)
        int_array = np.ndarray((ARRAY_SIZE,), dtype=np.int32, buffer=shm.buf)
        int_array += 5
        time.sleep(5)
        print('Python work')
        win32event.ReleaseSemaphore(cpp_sem, 1)
        win32api.CloseHandle(cpp_sem)
        print("Semaphore signaled! Continuing...")
        # Do something after the semaphore is released
        print("Work done. Waiting again...")  # Signal ready
    elif result == win32con.WAIT_ABANDONED:
        print("Semaphore abandoned! Exiting...")
        break  # Exit loop if abandoned
    else:
        print(f"Wait failed: {result}")
        break  # Exit loop


