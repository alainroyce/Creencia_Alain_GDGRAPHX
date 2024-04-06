#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
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
#include "obj_mesh.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class SceneLoaderClient {
public:
    SceneLoaderClient(std::shared_ptr<Channel> channel)
        : stub_(SceneLoader::NewStub(channel)) {}
    std::vector<ObjModel> ObjectList;
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
            ObjectList.push_back(objModel);
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

void drawObj(const ObjModel& objModel, GLuint& shaderProgram, GLuint& normalTransformLoc, GLuint& modelTransformLoc);
enum SceneState {
    SCENE_1,
    SCENE_2,
    SCENE_3,
    SCENE_4,
    SCENE_5,
    SCENE_6
};

SceneState currentScene = SCENE_5;

int main(int argc, char** argv) {
    // Specify the server address
    std::string server_address("localhost:50051");

    // Create channel arguments to configure maximum message size
    grpc::ChannelArguments channelArgs;
    // Set the maximum receive message size to a larger value (e.g., 10 MB)
    channelArgs.SetInt(GRPC_ARG_MAX_RECEIVE_MESSAGE_LENGTH, 10 * 1024 * 1024); // 10 MB

    // Create a channel to connect to the server with the specified arguments
    std::shared_ptr<Channel> channel = grpc::CreateCustomChannel(server_address,
        grpc::InsecureChannelCredentials(), channelArgs);

    // Create a client instance
    SceneLoaderClient client(channel);
    client.ObjectList;

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

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Parcm Creencia-Villegas", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    
    glfwSwapInterval(165/60); 

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


    ObjData earth;
    //backpack.textures = 
    LoadObjFile(&earth, "grass2/10450_Rectangular_Grass_Patch_v1_iterations-2.obj");
    GLfloat earthOffsets[] = { 0.0f, 0, 0 }; //x,z,y
    LoadObjToMemory(
        &earth,
        1.0f,
        earthOffsets
    );
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

    // define projection matrix
    glm::mat4 projection = glm::mat4(1.0f);

    //setup light shading; position of the flash light; changes
    GLuint lightPoscLoc = glGetUniformLocation(shaderProgram, "u_light_pos"); //changes
    GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "u_light_dir"); //changes
    GLuint diffuseTexLoc = glGetUniformLocation(shaderProgram, "texture_diffuse"); //changes1
    GLuint normalTexLoc = glGetUniformLocation(shaderProgram, "texture_normal"); //changes1

    glUniform1i(diffuseTexLoc, 0);//changes1
    glUniform1i(normalTexLoc, 1);//changes1

    glUniform3f(lightDirLoc, 1.0f, 1.0f, 1.0f); //changes

#pragma endregion

    // set bg color to green
    glClearColor(0.4f, 0.4f, 0.0f, 0.0f);
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

    //cameraMovement initialization
    cameraMovement::initialize();

    glfwSetCursorPosCallback(window, cameraMovement::getInstance()->mouse_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //depth testing
    glEnable(GL_DEPTH_TEST);

    //face culling
    glEnable(GL_CULL_FACE);

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
        if (ImGui::Button("LoadAll")) {
            currentScene = SCENE_6;
        }
        ImGui::End();


        if (ImGui::Begin("Profiler")) {

            // String ms_text = "Application Average " + std::to_string(1000 * EngineTime::getDeltaTime()) + " ms/frame (" + std::to_string(std::round(1 / EngineTime::getDeltaTime()))  + " FPS)";
            std::string ms_text = "Application Average " + std::to_string(ImGui::GetIO().Framerate / 1000.0f) + " ms/frame (" + std::to_string(ImGui::GetIO().Framerate) + " FPS)";
            ImGui::Text(ms_text.c_str());

            ImGui::End();
        }
#pragma region Viewport
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
#pragma endregion

#pragma region Projection
        // Perspective Projection
        projection = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 1000.0f);
        // Set projection matrix in shader
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
#pragma endregion

#pragma region View
        // increment rotation by deltaTime
        currentTime = glfwGetTime();
        deltaTime = currentTime - prevTime;

        glm::mat4 view = glm::lookAt(cameraMovement::getInstance()->cameraPos,
            cameraMovement::getInstance()->cameraPos + cameraMovement::getInstance()->cameraFront,
            cameraMovement::getInstance()->cameraUp);

        // w,a,s,d movement of the camera
        cameraMovement::getInstance()->moveCam(window, deltaTime);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
#pragma endregion

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#pragma region Draw
        //------------ start drawing here

        // deltaTime value
        currentTime = glfwGetTime();
        deltaTime = currentTime - prevTime;
        prevTime = currentTime;


        if (currentScene == SCENE_1) {
            drawObj(client.ObjectList[0], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[5], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[2], shaderProgram, normalTransformLoc, modelTransformLoc);
            std::cout << "Name: " << client.ObjectList[0].modelname() << std::endl;
        }
        else if (currentScene == SCENE_2) {
            drawObj(client.ObjectList[1], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[3], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[2], shaderProgram, normalTransformLoc, modelTransformLoc);
        }
        else if (currentScene == SCENE_3) {
            drawObj(client.ObjectList[4], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[2], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[4], shaderProgram, normalTransformLoc, modelTransformLoc);
        }
        else if (currentScene == SCENE_4) {

            drawObj(client.ObjectList[2], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[3], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[3], shaderProgram, normalTransformLoc, modelTransformLoc);
        }
        else if (currentScene == SCENE_5) {
            drawObj(client.ObjectList[5], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[2], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[1], shaderProgram, normalTransformLoc, modelTransformLoc);
        }
        else if (currentScene == SCENE_6)
        {
            drawObj(client.ObjectList[0], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[1], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[2], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[3], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[4], shaderProgram, normalTransformLoc, modelTransformLoc);
            drawObj(client.ObjectList[5], shaderProgram, normalTransformLoc, modelTransformLoc);
        }


        //--- stop drawing here ---
#pragma endregion

        glfwSwapBuffers(window);
        // listen for glfw input events
        glfwPollEvents();
    }
    return 0;
}

void drawObj(const ObjModel& objModel, GLuint& shaderProgram, GLuint& normalTransformLoc, GLuint& modelTransformLoc) {
    glUseProgram(shaderProgram);

    // Iterate over vertices and draw them
    for (const auto& vertex : objModel.vertices()) {
        // Create transformation matrix (identity for now)
        glm::mat4 trans = glm::mat4(1.0f);

        // Send transformation matrix to shader
        glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        // Draw vertex
        glBegin(GL_POINTS);
        glVertex3f(vertex.x(), vertex.y(), vertex.z());
        glEnd();
    }
}
