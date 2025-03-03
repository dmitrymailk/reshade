import win32event
import time
import win32api

SEMAPHORE_NAME = "my_semaphore"

def signal_semaphore():
    hSemaphore = win32event.OpenSemaphore(win32event.SYNCHRONIZE | win32event.EVENT_MODIFY_STATE, False, SEMAPHORE_NAME)
    win32event.ReleaseSemaphore(hSemaphore, 1)
    win32api.CloseHandle(hSemaphore)
    print("Python: Semaphore Signaled.")

if __name__ == "__main__":
    # Signal the semaphore twice with a delay
    signal_semaphore()
    # time.sleep(2) #Simulate some work
    signal_semaphore()