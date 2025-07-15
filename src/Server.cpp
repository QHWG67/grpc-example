// Include gRPC and Protobuf headers
#include <grpcpp/grpcpp.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/security/server_credentials.h>
#include <user.grpc.pb.h> // Ensure this header is included to use UserService::Service

// Define the UserServiceImpl class, inheriting from the gRPC-generated UserService::Service
class UserServiceImpl final : public example::UserService::Service {
    // Override the GetUser method to handle client requests and return responses
    grpc::Status GetUser(grpc::ServerContext* ctx, const example::UserRequest* req, example::UserResponse* res) override {
        res->set_name("Alice");  // Business logic: set the returned username to "Alice"
        res->set_email("Alie@google.com"); // Set the returned email address
        return grpc::Status::OK;  // Return success status
    }
};

int main(int argc, char** argv) {
    const std::string server_address("0.0.0.0:50051"); // Server listening address and port
    UserServiceImpl service; // Create an instance of the service

    grpc::ServerBuilder builder; // Create a gRPC server builder
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials()); // Add listening port and security credentials
    builder.RegisterService(&service); // Register the service instance

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart()); // Start the server
    std::cout << "Server listening on " << server_address << std::endl; // Output server startup information

    server->Wait(); // Block and wait for client requests
    return 0; // End of program
}