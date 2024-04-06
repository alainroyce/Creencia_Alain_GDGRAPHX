
#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "SceneObject.grpc.pb.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>




using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;


class SceneLoaderClient {
public:
    SceneLoaderClient(std::shared_ptr<Channel> channel)
        : stub_(SceneLoader::NewStub(channel)) {}

    void LoadObjects(const int integer) {
        Integer request;
        request.set_integer(integer);

        ClientContext context;
        std::unique_ptr<grpc::ClientReader<ObjModel>> reader(
            stub_->LoadObjects(&context, request));

        ObjModel objModel;
        while (reader->Read(&objModel)) {
            std::cout << "Received ObjModel: " << objModel.modelname() << std::endl;
            // Process objModel.vertices(), objModel.normals(), objModel.indices() etc.
        }

        Status status = reader->Finish();
        if (status.ok()) {
            std::cout << "LoadObjects rpc succeeded." << std::endl;
        }
        else {
            std::cerr << "LoadObjects rpc failed: " << status.error_message() << std::endl;
        }
    }



private:
    std::unique_ptr<SceneLoader::Stub> stub_;
};

int main(int argc, char** argv) {
    // Specify the server address
    std::string server_address("localhost:50051");

    // Create a channel to connect to the server
    grpc::ChannelArguments channelArgs;
    std::shared_ptr<Channel> channel = grpc::CreateCustomChannel(server_address,
        grpc::InsecureChannelCredentials(), channelArgs);

    // Create a client instance
    SceneLoaderClient client(channel);

    // Perform RPC calls
    client.LoadObjects(123);

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    //Create a GLFW window
        GLFWwindow * window = glfwCreateWindow(1280, 720, "Parcm", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!glfwWindowShouldClose(window)) {
      
       
    }

  
    return 0;
}
