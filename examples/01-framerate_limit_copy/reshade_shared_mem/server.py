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
SHARED_MEMORY_NAME = "shared_image"

ARRAY_SIZE = 1920*1080*4
# ARRAY_SIZE = 1920
SHARED_MEMORY_SIZE = ARRAY_SIZE * 1

if python_sem is None:
    print(f"Error creating semaphore: {win32event.GetLastError()}")
    exit(1)

print("Semaphore created in Python. Waiting for signals...")

global_counter = 0

while True:
    # Wait for the semaphore to be signaled
    result = win32event.WaitForSingleObject(python_sem, win32event.INFINITE)
    if result == win32con.WAIT_OBJECT_0:
        cpp_sem = win32event.OpenSemaphore(win32event.SYNCHRONIZE | win32event.EVENT_MODIFY_STATE, False, CppSemaphore,)
        # DO SOME WORK
        shm = multiprocessing.shared_memory.SharedMemory(name=SHARED_MEMORY_NAME, create=False, size=SHARED_MEMORY_SIZE)
        int_array = np.ndarray((ARRAY_SIZE,), dtype=np.uint8, buffer=shm.buf)
        int_array[:] = 255 - int_array 
        global_counter += 1
        print('Python work ', global_counter)
        win32event.ReleaseSemaphore(cpp_sem, 1)
       
        print("Work done. Waiting again...")  # Signal ready
    elif result == win32con.WAIT_ABANDONED:
        print("Semaphore abandoned! Exiting...")
        break  # Exit loop if abandoned
    else:
        print(f"Wait failed: {result}")
        break  # Exit loop


