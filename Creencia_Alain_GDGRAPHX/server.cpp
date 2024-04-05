#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <google/protobuf/empty.pb.h>
#include "objects.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using objects::Vertex;
using objects::Texture;
using objects::Material;
using objects::Object;
using objects::Scene;

// Logic and data behind the server's behavior.
class ObjectsServiceImpl final {
public:
    Status GetScene(ServerContext* context, const google::protobuf::Empty* request,
        Scene* response) {
        // Here you would implement logic to populate the 'response' with scene data
        // For demonstration purposes, let's create a simple scene with one object

        Object* obj = response->add_objects();
        obj->set_name("Cube");

        // Adding vertices
        Vertex* v1 = obj->add_vertices();
        v1->set_x(0.0);
        v1->set_y(0.0);
        v1->set_z(0.0);

        Vertex* v2 = obj->add_vertices();
        v2->set_x(1.0);
        v2->set_y(0.0);
        v2->set_z(0.0);

        Vertex* v3 = obj->add_vertices();
        v3->set_x(1.0);
        v3->set_y(1.0);
        v3->set_z(0.0);

        // Adding indices
        obj->add_indices(0);
        obj->add_indices(1);
        obj->add_indices(2);

        // Adding material
        Material* mat = obj->mutable_material();
        mat->set_name("DefaultMaterial");
        Texture* tex = mat->mutable_diffuse_texture();
        tex->set_filename("default_texture.jpg");

        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    ObjectsServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
