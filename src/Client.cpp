// Include necessary headers
#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "user.grpc.pb.h"  // Auto-generated gRPC and Protobuf file

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using example::UserService;
using example::UserRequest;
using example::UserResponse;

// Define the UserClient class to interact with the gRPC server
class UserClient {
public:
    // Constructor initializes the stub for the UserService
    UserClient(std::shared_ptr<Channel> channel)
        : stub_(UserService::NewStub(channel)) {}

    // Method to call the GetUser RPC
    std::string GetUser(const uint32_t user_id) {
        // Create a request object
        UserRequest request;
        request.set_id(user_id);  // Set the user ID in the request

        // Create a response object
        UserResponse response;

        // Create a context for the RPC
        ClientContext context;

        // Call the remote GetUser method
        Status status = stub_->GetUser(&context, request, &response);

        // Check the status of the RPC call
        if (status.ok()) {
            return "Name: " + response.name() + ", Email: " + response.email();  // Return the name and email from the response
        } else {
            std::cerr << "RPC failed: " << status.error_message() << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<UserService::Stub> stub_; // Stub to interact with the server
};

int main(int argc, char** argv) {
    // Create a client and connect to the server
    UserClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    // Call the GetUser method
    uint32_t user_id = 12345;  // Example user ID
    std::string user_info = client.GetUser(user_id);

    // Print the result
    std::cout << "User info: " << user_info << std::endl;

    return 0;
}