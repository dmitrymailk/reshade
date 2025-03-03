import grpc
from concurrent import futures
import image_service_pb2
import image_service_pb2_grpc
from PIL import Image
import io
import numpy as np
class ImageServiceServicer(image_service_pb2_grpc.ImageServiceServicer):
    def SendImage(self, request, context):
        try:
            image_data = request.image_data
            image_format = request.image_format

            # Decode the image data
            with open('received_image_raw.bytes', 'wb')as f:
                f.write(image_data)

            print(f"Received image in {image_format} format.  Processed successfully.")

            return image_service_pb2.ImageResponse(message="Image received and processed successfully!", success=True)

        except Exception as e:
            print(f"Error processing image: {e}")
            context.abort(grpc.StatusCode.INTERNAL, f"Image processing failed: {e}") # use context.abort for errors
            return image_service_pb2.ImageResponse(message=f"Error processing image: {e}", success=False)


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    image_service_pb2_grpc.add_ImageServiceServicer_to_server(ImageServiceServicer(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    print("Server started, listening on port 50051.")
    server.wait_for_termination()

if __name__ == '__main__':
    serve()