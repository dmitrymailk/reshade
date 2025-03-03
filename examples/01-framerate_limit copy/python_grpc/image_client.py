import grpc
import image_service_pb2
import image_service_pb2_grpc
from PIL import Image
import io

def run():
    # Read the image file
    image_path = "test_image.jpg" # Replace with your image path
    try:
        with open(image_path, "rb") as image_file:
            image_data = image_file.read()

        image = Image.open(image_path)
        image_format = image.format.lower()  # Get the image format (e.g., jpeg, png)

        # Create a gRPC channel to connect to the server
        with grpc.insecure_channel('localhost:50051') as channel:
            stub = image_service_pb2_grpc.ImageServiceStub(channel)

            # Create an ImageRequest message
            request = image_service_pb2.ImageRequest(image_data=image_data, image_format=image_format)

            # Send the image to the server
            try:
                response = stub.SendImage(request)

                if response.success:
                    print("Server response:", response.message)
                else:
                    print("Server error:", response.message)  # Handle server-side errors
            except grpc.RpcError as e:
                print(f"gRPC error: {e.code()} {e.details()}") # Handle client-side gRPC errors
    except FileNotFoundError:
        print(f"Error: Image file not found at {image_path}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")


if __name__ == '__main__':
    run()