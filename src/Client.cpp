// Include necessary headers
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>
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

    // Method to call the StreamUsers RPC
    void StreamUsers(const uint32_t user_id) {
        // Create a request object
        UserRequest request;
        request.set_id(user_id);  // Set the user ID in the request

        // Create a context for the RPC
        ClientContext context;

        // Create a reader for the server-streaming RPC
        std::unique_ptr<grpc::ClientReader<UserResponse>> reader(
            stub_->StreamUsers(&context, request));

        // Read responses from the server
        UserResponse response;
        while (reader->Read(&response)) {
            std::cout << "Name: " << response.name() << ", Email: " << response.email() << std::endl;
        }

        // Check the status of the RPC
        Status status = reader->Finish();
        if (!status.ok()) {
            std::cerr << "StreamUsers RPC failed: " << status.error_message() << std::endl;
        }
    }

    // Method to call the UploadUsers RPC
    void UploadUsers(const std::vector<uint32_t>& user_ids) {
        // Create a context for the RPC
        ClientContext context;

        // Create a response object
        UserResponse response;

        // Create a writer for the client-streaming RPC
        std::unique_ptr<grpc::ClientWriter<UserRequest>> writer(
            stub_->UploadUsers(&context, &response));

        // Send each UserRequest to the server
        for (const auto& user_id : user_ids) {
            UserRequest request;
            request.set_id(user_id);
            if (!writer->Write(request)) {
                std::cerr << "Failed to write request for user ID: " << user_id << std::endl;
                break;
            }
        }

        // Close the writer and check the status of the RPC
        writer->WritesDone();
        Status status = writer->Finish();
        if (status.ok()) {
            std::cout << "Concatenated Names: " << response.name() << std::endl;
            std::cout << "Concatenated Emails: " << response.email() << std::endl;
        } else {
            std::cerr << "UploadUsers RPC failed: " << status.error_message() << std::endl;
        }
    }

    // Method to call the ChatUsers RPC
    void ChatUsers(const std::vector<uint32_t>& user_ids) {
        // Create a context for the RPC
        ClientContext context;

        // Create a reader-writer for the bidirectional streaming RPC
        std::unique_ptr<grpc::ClientReaderWriter<UserRequest, UserResponse>> stream(
            stub_->ChatUsers(&context));

        // Start a thread to send requests to the server
        std::thread writer_thread([&]() {
            for (const auto& user_id : user_ids) {
                UserRequest request;
                request.set_id(user_id);
                if (!stream->Write(request)) {
                    std::cerr << "Failed to write request for user ID: " << user_id << std::endl;
                    break;
                }
            }
            stream->WritesDone(); // Indicate that no more requests will be sent
        });

        // Read responses from the server
        UserResponse response;
        while (stream->Read(&response)) {
            std::cout << "Received Response - Name: " << response.name()
                      << ", Email: " << response.email() << std::endl;
        }

        // Wait for the writer thread to finish
        writer_thread.join();

        // Check the status of the RPC
        Status status = stream->Finish();
        if (!status.ok()) {
            std::cerr << "ChatUsers RPC failed: " << status.error_message() << std::endl;
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

    // Call the StreamUsers method
    std::cout << "Streaming user info:" << std::endl;
    client.StreamUsers(user_id);

    // Call the UploadUsers method
    std::vector<uint32_t> user_ids = {12345, 67890, 54321};  // Example user IDs
    std::cout << "Uploading user info:" << std::endl;
    client.UploadUsers(user_ids);

    // Call the ChatUsers method
    std::cout << "Chatting with users:" << std::endl;
    client.ChatUsers(user_ids);

    return 0;
}