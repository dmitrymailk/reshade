#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "image_service.grpc.pb.h" // Replace with actual path
#include "image_service.pb.h"    // Replace with actual path

using grpc::Channel;
using grpc::ClientContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using image_service::ImageService;
using image_service::ImageRequest;
using image_service::ImageResponse;

// **Server Implementation**
class ImageServiceImpl final : public ImageService::Service {
	Status SendImage(ServerContext *context, const ImageRequest *request,
					 ImageResponse *response) override {
		const std::string &image_data = request->image_data();
		const std::string &image_format = request->image_format();

		std::cout << "Received image: " << image_data.size() << " bytes, format: "
			<< image_format << std::endl;

		// **Example: Save the image to a file**
		std::string filename = "received_image." + image_format;
		std::ofstream output(filename, std::ios::binary);
		if (output.is_open()) {
			output.write(image_data.data(), image_data.size());
			output.close();
			std::cout << "Image saved to: " << filename << std::endl;
			response->set_message("Image received and saved successfully.");
			response->set_success(true);
		}
		else {
			std::cerr << "Error saving image to file!" << std::endl;
			response->set_message("Error saving image.");
			response->set_success(false);
			return Status(grpc::StatusCode::INTERNAL, "Failed to save image");
		}

		return Status::OK;
	}
};


// **Client Implementation**
class ImageServiceClient {
public:
	ImageServiceClient(std::shared_ptr<Channel> channel)
		: stub_(ImageService::NewStub(channel)) {
	}

	bool SendImage(const std::string &image_path, const std::string &image_format) {
		ImageRequest request;
		ImageResponse response;
		ClientContext context;

		// Read the image file into a byte vector
		std::ifstream input(image_path, std::ios::binary);
		if (!input.is_open()) {
			std::cerr << "Error opening image file: " << image_path << std::endl;
			return false;
		}

		std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {}); // Read entire file

		// Set the image data and format in the request
		request.set_image_data(buffer.data(), buffer.size());  // Crucial: Use data() and size()!
		request.set_image_format(image_format);

		// Make the RPC call
		Status status = stub_->SendImage(&context, request, &response);

		if (status.ok()) {
			std::cout << "Server response: " << response.message() << std::endl;
			return response.success();
		}
		else {
			std::cerr << "SendImage failed: " << status.error_code() << ": "
				<< status.error_message() << std::endl;
			return false;
		}
	}

private:
	std::unique_ptr<ImageService::Stub> stub_;
};


// **Main Function (Example)**
int main(int argc, char **argv) {
	// **Server**
	std::string server_address("0.0.0.0:50051");
	ImageServiceImpl service;

	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	// **Client**
	ImageServiceClient client(grpc::CreateChannel(
		"localhost:50051", grpc::InsecureChannelCredentials()));

	// Example Usage
	// Create a dummy image file for testing (or use a real one)
	std::ofstream dummy_image("test_image.jpg", std::ios::binary);
	dummy_image << "This is a dummy JPEG image file.";
	dummy_image.close();

	bool success = client.SendImage("test_image.jpg", "jpeg");

	if (success) {
		std::cout << "Image sent successfully!" << std::endl;
	}
	else {
		std::cout << "Image sending failed." << std::endl;
	}

	server->Wait();
	return 0;
}
