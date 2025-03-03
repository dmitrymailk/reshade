import grpc
import image_service_pb2
import image_service_pb2_grpc
import io
from PIL import Image

def send_image(image_path, image_format):
    with open(image_path, 'rb') as f:
        image_data = f.read()

    with grpc.insecure_channel('localhost:50051') as channel:
        stub = image_service_pb2_grpc.ImageServiceStub(channel)
        request = image_service_pb2.ImageRequest(image_data=image_data, image_format=image_format)
        response = stub.SendImage(request)

        if response.success:
            received_image_data = response.image_data  # Access image data from the dedicated field

            # Save the received image to a file
            output_path = "received_image.jpg"
            with open(output_path, 'wb') as outfile:
                outfile.write(received_image_data)
            print(f"Image saved to: {output_path}")
            print("Server message: " + response.message)
        else:
            print("Server response: " + response.message)

        print("Success: " + str(response.success))

if __name__ == '__main__':
    image_path = "test_image.jpg"
    image_format = "jpeg"

    # try:
    #     img = Image.new('RGB', (100, 100), color='red')
    #     img.save(image_path, "JPEG")
    # except FileNotFoundError:
    #     print("Warning: Could not create dummy JPEG.  Ensure PIL is installed.")

    send_image(image_path, image_format)