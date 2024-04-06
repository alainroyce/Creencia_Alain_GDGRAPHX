#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "SceneObject.grpc.pb.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h" 

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::StatusCode;


class SceneLoaderServiceImpl final : public SceneLoader::Service {
public:
    Status LoadObjects(ServerContext* context, const Integer* request, grpc::ServerWriter<ObjModel>* writer) override {
        std::string objFilename = "C:\\Users\\alain\\source\\repos\\SFML_AlainCreencia\\Creencia_Alain_GDGRAPHX\\Creencia_Alain_GDGRAPHX\\Assets\\cow.obj"; // Replace with the path to your obj file
        std::string mtlBaseDir = "path/to/your/materials/";

        // Load .obj file using tinyobj_loader.h
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objFilename.c_str(), mtlBaseDir.c_str());

        if (!err.empty()) {
            std::cerr << err << std::endl;
        }

        if (!ret) {
            return Status(StatusCode::INTERNAL, "Failed to load obj file");
        }

        // Iterate over vertices and send them to the client
        for (const auto& shape : shapes) {
            ObjModel objModel;
            objModel.set_modelname(shape.name);

            for (const auto& index : shape.mesh.indices) {
                Vector3* vertex = objModel.add_vertices();
                vertex->set_x(attrib.vertices[3 * index.vertex_index + 0]);
                vertex->set_y(attrib.vertices[3 * index.vertex_index + 1]);
                vertex->set_z(attrib.vertices[3 * index.vertex_index + 2]);
            }

            writer->Write(objModel);
        }

        return Status::OK;
    }
};


void RunServer() {
    std::string server_address("0.0.0.0:50051");
    SceneLoaderServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
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
