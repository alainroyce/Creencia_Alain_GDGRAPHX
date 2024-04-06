#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Classes/cameraMovement.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <grpcpp/grpcpp.h>
#include "SceneObject.grpc.pb.h"



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

//void drawObj(glm::mat4& trans, ObjData& structure, GLuint& shaderProgram, GLuint& normalTransformLoc, GLuint& modelTransformLoc,
  //  float deg, std::vector<glm::vec3>& Vec3); //Vec3 = rotate axis, translation, scaling

enum SceneState {
    SCENE_1,
    SCENE_2,
    SCENE_3,
    SCENE_4,
    SCENE_5,
    SCENE_6
};

SceneState currentScene = SCENE_1;

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

#pragma region Initialization
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Set GLFW window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Parcm Creencia-Villegas", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
#pragma endregion
#pragma region Shader Loading

    //LoadSkybox shader
    GLuint skyboxShderProgram = LoadShaders("Shaders/skybox_vertex.shader", "Shaders/skybox_fragment.shader"); //changes

    //used directional light for light source
    GLuint shaderProgram = LoadShaders("Shaders/phong_vertex.shader", "Shaders/phong_directional_fragment.shader"); //changes
    glUseProgram(shaderProgram);

    GLuint colorLoc = glGetUniformLocation(shaderProgram, "u_color");
    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

    // initialize MVP
    GLuint modelTransformLoc = glGetUniformLocation(shaderProgram, "u_model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "u_view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "u_projection");

    //initialize normal transformation; changes
    GLuint normalTransformLoc = glGetUniformLocation(shaderProgram, "u_normal"); //changes
    GLuint cameraPosLoc = glGetUniformLocation(shaderProgram, "u_camera_pos"); //changes
    GLuint ambientColorLoc = glGetUniformLocation(shaderProgram, "u_ambient_color"); //changes
    glUniform3f(ambientColorLoc, 0.1f, 0.1f, 0.1f); //changes

    //3 transformation for the 3 models
    glm::mat4 trans = glm::mat4(1.0f); // identity
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    glm::mat4 trans1 = glm::mat4(1.0f); // identity
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans1));

    // define projection matrix
    glm::mat4 projection = glm::mat4(1.0f);
    //glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //setup light shading; position of the flash light; changes
    GLuint lightPoscLoc = glGetUniformLocation(shaderProgram, "u_light_pos"); //changes
    //glUniform3f(lightPoscLoc, 0.0f, 1.0f, 0.0f);
    GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "u_light_dir"); //changes
    GLuint diffuseTexLoc = glGetUniformLocation(shaderProgram, "texture_diffuse"); //changes1
    GLuint normalTexLoc = glGetUniformLocation(shaderProgram, "texture_normal"); //changes1

    glUniform1i(diffuseTexLoc, 0);//changes1
    glUniform1i(normalTexLoc, 1);//changes1

    glUniform3f(lightPoscLoc, trans1[3][0], trans1[3][1] + 1.0f, trans1[3][1]); //changes //x,z,y
    glUniform3f(lightDirLoc, 1.0f, 1.0f, 1.0f); //changes

#pragma endregion

    // set bg color to green
   //glClearColor(0.4f, 0.4f, 0.0f, 0.0f);
    // timer for the switching view
    float changeViewTimer = 0.0f;
    float changeView = 3.0f;
    bool isChange = false;
    float currentTime = glfwGetTime();
    float prevTime = 0.0f;
    float deltaTime = 0.0f;

    //var for wasd movement 
    float Zvalue;
    float Xvalue;
    float Yvalue;
   

	while (!glfwWindowShouldClose(window)) {

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ImGui scene buttons
		ImGui::Begin("Scenes");



		if (ImGui::Button("Scene 1")) {
			currentScene = SCENE_1;
		}
		if (ImGui::Button("Scene 2")) {
			currentScene = SCENE_2;
		}
		if (ImGui::Button("Scene 3")) {
			currentScene = SCENE_3;
		}
		if (ImGui::Button("Scene 4")) {
			currentScene = SCENE_4;
		}
		if (ImGui::Button("Scene 5")) {
			currentScene = SCENE_5;
		}
		ImGui::End();

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


#pragma region Draw
		//------------ start drawing here

		//deltaTime value
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		if (currentScene == SCENE_1) {
		
		}
		else if (currentScene == SCENE_2) {
			
		}
		else if (currentScene == SCENE_3) {
			
		}
		else if (currentScene == SCENE_4) {
		

		}
		else if (currentScene == SCENE_5) {

		}

		//--- stop drawing here ---
#pragma endregion

		glfwSwapBuffers(window);
		//listen for glfw input events
		glfwPollEvents();
	}
    return 0;
}
