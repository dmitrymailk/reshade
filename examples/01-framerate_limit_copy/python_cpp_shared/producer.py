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
# finally:
#     # --- Cleanup ---
#     if 'shm' in locals():
#         shm.close()  # Detach
#     if 'hSemaphore' in locals() and hSemaphore is not None:
#         win32api.CloseHandle(hSemaphore)  # Close the semaphore