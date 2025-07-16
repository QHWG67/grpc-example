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

    // Implement the StreamUsers method to return a stream of UserResponse
    grpc::Status StreamUsers(grpc::ServerContext* ctx, const example::UserRequest* req, grpc::ServerWriter<example::UserResponse>* writer) override {
        for (int i = 0; i < 5; ++i) { // Example: send 5 responses
            example::UserResponse response;
            response.set_name("User" + std::to_string(i)); // Set a unique name for each response
            response.set_email("user" + std::to_string(i) + "@google.com"); // Set a unique email for each response
            writer->Write(response); // Send the response to the client
        }
        return grpc::Status::OK; // Return success status
    }

    // Implement the UploadUsers method to handle a stream of UserRequest and return a UserResponse
    grpc::Status UploadUsers(grpc::ServerContext* ctx, grpc::ServerReader<example::UserRequest>* reader, example::UserResponse* res) override {
        std::string concatenated_names;
        std::string concatenated_emails;
        example::UserRequest request;

        while (reader->Read(&request)) { // Read each UserRequest from the stream
            concatenated_names += "User" + std::to_string(request.id()) + ", ";
            concatenated_emails += "user" + std::to_string(request.id()) + "@google.com, ";
        }

        res->set_name(concatenated_names); // Set concatenated names in the response
        res->set_email(concatenated_emails); // Set concatenated emails in the response
        return grpc::Status::OK; // Return success status
    }

    // Implement the ChatUsers method to handle bidirectional streaming
    grpc::Status ChatUsers(grpc::ServerContext* ctx, grpc::ServerReaderWriter<example::UserResponse, example::UserRequest>* stream) override {
        example::UserRequest request;
        while (stream->Read(&request)) { // Read each UserRequest from the stream
            example::UserResponse response;
            response.set_name("Echo: User" + std::to_string(request.id())); // Echo the user ID in the response
            response.set_email("Echo: user" + std::to_string(request.id()) + "@google.com"); // Echo the email in the response
            stream->Write(response); // Send the response back to the client
        }
        return grpc::Status::OK; // Return success status
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