cmake_minimum_required( VERSION 3.1 )

project( grpc_example )

find_package(gRPC CONFIG REQUIRED)

find_package( Protobuf REQUIRED )

add_executable( server server.cpp SceneObject.grpc.pb.cc SceneObject.pb.cc)

add_executable( client client.cpp SceneObject.grpc.pb.cc SceneObject.pb.cc)

target_link_libraries( server PRIVATE gRPC::grpc++ gRPC::grpc++_reflection gRPC::gpr gRPC::grpc gRPC::grpc++ protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite )

target_link_libraries( client PRIVATE gRPC::grpc++ gRPC::grpc++_reflection gRPC::gpr gRPC::grpc gRPC::grpc++ protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite )