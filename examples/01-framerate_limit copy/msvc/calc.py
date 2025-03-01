import pickle

def add(x, y, array):
    print("array 123", array)
    array[0][0] = 666
    with open('D:\\programming\\auto_remaster\\reshade\examples\\01-framerate_limit copy\\msvc\\temp.pickle', 'wb',) as f:
        # Use pickle.dump() to serialize the data and write it to the file
        pickle.dump(array, f)
    return array


# test new text