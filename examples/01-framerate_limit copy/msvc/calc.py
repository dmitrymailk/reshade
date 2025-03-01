import numpy as np
global_counter = 0
def add(array: np.ndarray):
    array = array.reshape(1080, 1920, 4)
    global global_counter 
    global_counter += 1
    with open('D:\\programming\\auto_remaster\\reshade\\examples\\01-framerate_limit copy\\msvc\\counter', 'w') as f:
        f.write(str(global_counter))
    array = 255 - array
    array = array.flatten()
    return array