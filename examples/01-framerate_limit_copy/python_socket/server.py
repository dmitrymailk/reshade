import socket
import struct
import os
from PIL import Image
import io

def handle_client(conn, addr, upload_dir):
    """Handles communication with a single client."""
    print(f"Connected by {addr}")

    try:
        # 1. Receive the image length (4 bytes, big-endian)
        length_bytes = conn.recv(4)
        if not length_bytes:  # Client disconnected
            print(f"Client {addr} disconnected.")
            return

        image_length = struct.unpack('>I', length_bytes)[0]  # Unpack from big-endian unsigned int
        print(f"Image length: {image_length} bytes")

        # 2. Receive the image data
        image_data = b''
        bytes_received = 0
        while bytes_received < image_length:
            chunk = conn.recv(min(image_length - bytes_received, 4096*40)) # Read up to 4KB at a time
            if not chunk:
                print(f"Connection dropped while receiving data from {addr}")
                return
            image_data += chunk
            bytes_received += len(chunk)
        io.BytesIO(image_data)
        print(f"Received {bytes_received} bytes of image data.")

        # 3. Save the image (or process it)
        try:
            # img = Image.open(io.BytesIO(image_data))
            # filename = f"received_image_{addr[1]}.jpg"  # Unique filename
            # filepath = os.path.join(upload_dir, filename)
            # img.save(filepath, "JPEG")
            # print(f"Image saved to {filepath}")
            conn.sendall(b"Image received successfully")  #Send a confirmation
        except Exception as e:
            print(f"Error saving image: {e}")
            conn.sendall(b"Error saving image") # Send Error to client

    except Exception as e:
        print(f"Error handling client {addr}: {e}")
    finally:
        conn.close()

def start_server(host, port, upload_dir="uploads"):
    """Starts the TCP server."""
    os.makedirs(upload_dir, exist_ok=True)  # Create upload directory if it doesn't exist

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((host, port))
        s.listen()
        print(f"Server listening on {host}:{port}")

        while True:
            conn, addr = s.accept()
            handle_client(conn, addr, upload_dir)

if __name__ == "__main__":
    HOST = "localhost"
    PORT = 12345
    start_server(HOST, PORT)