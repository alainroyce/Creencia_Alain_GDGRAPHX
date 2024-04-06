#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
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

std::vector<std::string> GetObjFilesInDirectory(const std::string& directory) {
    std::vector<std::string> objFiles;

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((directory + "\\*.obj").c_str(), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            objFiles.push_back(directory + "\\" + findFileData.cFileName);
        } while (FindNextFile(hFind, &findFileData) != 0);
        FindClose(hFind);
    }

    return objFiles;
}

class SceneLoaderServiceImpl final : public SceneLoader::Service {
public:
    Status LoadObjects(ServerContext* context, const Integer* request, grpc::ServerWriter<ObjModel>* writer) override {
        std::string objDirectory = "C:\\Users\\alain\\source\\repos\\SFML_AlainCreencia\\Creencia_Alain_GDGRAPHX\\Creencia_Alain_GDGRAPHX\\Assets\\ModelsParcm";

        // Get OBJ files in the directory
        std::vector<std::string> objFiles = GetObjFilesInDirectory(objDirectory);

        // Load and process each OBJ file
        int i = 0;
        for (const auto& objFile : objFiles) {
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string warn;
            std::string err;

            bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objFile.c_str(), nullptr);

            if (!err.empty()) {
                std::cerr << err << std::endl;
            }

            if (!ret) {
                return Status(StatusCode::INTERNAL, "Failed to load obj file");
            }

            // Iterate over vertices and send them to the client
            for (const auto& shape : shapes) {
                ObjModel objModel;
                objModel.set_modelname("Object_" + std::to_string(i));
                std::cout << "name: " << objModel.modelname() << std::endl;
                for (const auto& index : shape.mesh.indices) {
                    Vector3* vertex = objModel.add_vertices();
                    vertex->set_x(attrib.vertices[3 * index.vertex_index + 0]);
                    vertex->set_y(attrib.vertices[3 * index.vertex_index + 1]);
                    vertex->set_z(attrib.vertices[3 * index.vertex_index + 2]);
                }

                writer->Write(objModel);
                i++;
            }
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
