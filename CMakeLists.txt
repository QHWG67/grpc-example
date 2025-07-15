# Minimum required version of CMake
cmake_minimum_required(VERSION 3.25)

# Define the project name
project(grpc_example)

# Find required packages for Protobuf and gRPC
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# Include directories for Protobuf and gRPC
include_directories(${Protobuf_INCLUDE_DIRS} ${gRPC_INCLUDE_DIRS})

# Locate the gRPC C++ plugin executable
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

# Check if the gRPC plugin executable is found
if(GRPC_CPP_PLUGIN_EXECUTABLE)
    message(STATUS "Found grpc_cpp_plugin: ${GRPC_CPP_PLUGIN_EXECUTABLE}")
else()
    message(FATAL_ERROR "grpc_cpp_plugin not found!")
endif()

# Define the directory containing the .proto files
set(PROTO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# List of .proto files to process
set(PROTO_FILES
    ./proto/user.proto
)

# Process each .proto file
foreach(_protoFile ${PROTO_FILES})
    get_filename_component(_absProtoFile ${_protoFile} ABSOLUTE) # Get absolute path of the .proto file
    get_filename_component(_protoPath ${_absProtoFile} PATH) # Get the directory of the .proto file
    string(REPLACE ".proto" ".grpc.pb.cc" _grpcSrc ${_protoFile}) # Generate .grpc.pb.cc file name
    string(REPLACE ".proto" ".grpc.pb.h" _grpcHdr ${_protoFile}) # Generate .grpc.pb.h file name

    message(STATUS ".grpc.pb.h: ${_grpcHdr}") # Output the generated .grpc.pb.h file name

    # Add custom command to compile gRPC services
    add_custom_command(
        OUTPUT "${_grpcSrc}" "${_grpcHdr}"
        COMMAND ${Protobuf_PROTOC_EXECUTABLE}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}/proto"
        -I ${_protoPath}
        --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${_absProtoFile}"
        DEPENDS ${_absProtoFile})
    list(APPEND GRPC_SRCS ${_grpcSrc}) # Append generated source files to the list
    list(APPEND GRPC_HDRS ${_grpcHdr}) # Append generated header files to the list
endforeach()

# Generate C++ files from .proto files
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES} PROTOC_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/proto")

# Include the directory containing generated files
include_directories(${CMAKE_CURRENT_BINARY_DIR}/proto)

# Define the server executable
add_executable(server ./src/Server.cpp ${PROTO_SRCS} ./proto/user.grpc.pb.cc)

# Define the client executable
add_executable(client ./src/Client.cpp ${PROTO_SRCS} ./proto/user.grpc.pb.cc)

# Link libraries for the server
target_link_libraries(server
    grpc++ # gRPC C++ library
    grpc # gRPC core library
    protobuf # Protobuf library
    gpr # gRPC runtime library
    absl_synchronization # Abseil synchronization library
)

# Link libraries for the client
target_link_libraries(client
    grpc++ # gRPC C++ library
    grpc # gRPC core library
    protobuf # Protobuf library
    gpr # gRPC runtime library
    absl_synchronization # Abseil synchronization library
)