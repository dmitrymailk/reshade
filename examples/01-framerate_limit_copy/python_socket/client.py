import socket
import struct
import os

def send_image(host, port, image_data):
    """Sends an image over a TCP socket."""
    try:
        # with open(image_path, 'rb') as f:
        #     image_data = f.read()

        image_length = len(image_data)

        # 1. Send the image length (4 bytes, big-endian)
        length_bytes = struct.pack('>I', image_length) # Pack as big-endian unsigned int
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((host, port))
            s.sendall(length_bytes)

            # 2. Send the image data
            s.sendall(image_data)

            #Receive Confirmation:
            confirmation = s.recv(1024)
            # print(f"Server Confirmation: {confirmation.decode()}")

    except FileNotFoundError:
        print(f"Error: Image file not found at ")
    except socket.error as e:
        print(f"Socket error: {e}")

if __name__ == "__main__":
    HOST = "localhost"
    PORT = 12345
    IMAGE_PATH = "test_image.jpg"  # Replace with the path to your image

    # Create a dummy test image if it doesn't exist
    from PIL import Image
    import time
    start = time.time()
    with open(IMAGE_PATH, 'rb') as f:
        image_data = f.read()
    for _ in range(1000):
        send_image(HOST, PORT, image_data)
    total = time.time() - start
    print('end', total, total/1000)