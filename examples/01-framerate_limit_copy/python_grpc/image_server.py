import grpc
from concurrent import futures
import image_service_pb2
import image_service_pb2_grpc
import io
from PIL import Image

class ImageService(image_service_pb2_grpc.ImageServiceServicer):
    def SendImage(self, request, context):
        try:
            image_data = request.image_data
            image_format = request.image_format
            io.BytesIO(image_data)
            # try:
            #     img = Image.open(io.BytesIO(image_data))
            #     img.verify()
            # except Exception as e:
            #     return image_service_pb2.ImageResponse(message=f"Error processing image: {str(e)}", success=False, image_data=b'') #Return empty bytes

            print(f"Received image with format: {image_format}")

            #Returning the image data directly in the image_data field
            return image_service_pb2.ImageResponse(message="Image successfully processed", success=True, image_data=image_data)

        except Exception as e:
            print(f"An error occurred: {e}")
            return image_service_pb2.ImageResponse(message=f"Error processing request: {str(e)}", success=False, image_data=b'') #Return empty bytes


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    image_service_pb2_grpc.add_ImageServiceServicer_to_server(ImageService(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    server.wait_for_termination()


if __name__ == '__main__':
    print("Starting gRPC image service...")
    serve()