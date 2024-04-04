#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Classes/cameraMovement.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "obj_mesh.h"
#include "shader.h"
#include "skybox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Method declarations
void drawObj(glm::mat4& trans, ObjData& structure, GLuint& shaderProgram, GLuint& normalTransformLoc, GLuint& modelTransformLoc,
    float deg, std::vector<glm::vec3>& Vec3); // Vec3 = rotate axis, translation, scaling

int main() {
    stbi_set_flip_vertically_on_load(true);

#pragma region Initialization
    // Initialize GLFW
    if (glfwInit() != GLFW_TRUE) {
        fprintf(stderr, "Failed to initialize GLFW!\n");
        return -1;
    }

    // Set OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window;
    window = glfwCreateWindow(1280, 720, "ImGui Scenes", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create window!\n");
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW!\n");
        return -1;
    }
#pragma endregion

#pragma region Mesh Loading
    ObjData earth;
    //backpack.textures = 
    LoadObjFile(&earth, "grass2/10450_Rectangular_Grass_Patch_v1_iterations-2.obj");
    GLfloat earthOffsets[] = { 0.0f, 0, 0 }; //x,z,y
    LoadObjToMemory(
        &earth,
        1.0f,
        earthOffsets
    );

    ObjData moon;
    //backpack.textures = 
    LoadObjFile(&moon, "earth/Earth.obj");
    GLfloat moonOffsets[] = { 0.0f, 0.0f, 0.0f };
    LoadObjToMemory(
        &moon,
        1.0f,
        moonOffsets
    );
    ObjData barn;
    //backpack.textures = 
    LoadObjFile(&barn, "Powerplant/10078_Nuclear_Power_Plant_v1_L3.obj");
    GLfloat barnOffsets[] = { 0.0f, 0.0f, 0.0f };
    LoadObjToMemory(
        &barn,
        1.0f,
        barnOffsets
    );

    ObjData structure;
    //backpack.textures = 
    LoadObjFile(&structure, "structure/10063_Church_v3.obj");
    GLfloat structureOffsets[] = { 0.0f, 0.0f, 0.0f };
    LoadObjToMemory(
        &structure,
        1.0f,
        structureOffsets
    );
    ObjData structure2;
    //backpack.textures = 
    LoadObjFile(&structure2, "structure2/13943_Flatiron_Building_v1_l1.obj");
    GLfloat structure2Offsets[] = { 0.0f, 0.0f, 0.0f };
    LoadObjToMemory(
        &structure2,
        1.0f,
        structure2Offsets
    );
    ObjData structure3;
    //backpack.textures = 
    LoadObjFile(&structure3, "structure3/10060_Big_Box_Storefront_V4_L3.obj");
    GLfloat structure3Offsets[] = { 0.0f, 0.0f, 0.0f };
    LoadObjToMemory(
        &structure3,
        1.0f,
        structure3Offsets
    );
    ObjData structure4;
    //backpack.textures = 
    LoadObjFile(&structure4, "structure4/10093_Wembley_stadion_V3_Iteration0.obj");
    GLfloat structure4Offsets[] = { 0.0f, 0.0f, 0.0f };
    LoadObjToMemory(
        &structure4,
        1.0f,
        structure4Offsets
    );
    ObjData road;
    //backpack.textures = 
    LoadObjFile(&road, "road/10562_RoadSectionCross_v2-L3.obj");
    GLfloat roadOffsets[] = { 0.0f, 0.0f, 0.0f };
    LoadObjToMemory(
        &road,
        1.0f,
        roadOffsets
    );

    std::vector<std::string> faces{
        "right.png",
        "left.png",
        "bottom.png",
        "top.png",
        "front.png",
        "back.png"
    };
    SkyBoxData skybox = LoadSkybox("Assets/skybox", faces); //changes
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
    glm::mat4 trans2 = glm::mat4(1.0f); // identity
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans2));
    glm::mat4 trans3 = glm::mat4(1.0f); // identity
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans3));
    glm::mat4 trans4 = glm::mat4(1.0f); // identity
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans4));
    glm::mat4 trans5 = glm::mat4(1.0f); // identity
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans5));
    glm::mat4 trans6 = glm::mat4(1.0f); // identity
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans6));
    glm::mat4 trans7 = glm::mat4(1.0f); // identity
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans7));

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

    // Set background color
    glClearColor(0.4f, 0.4f, 0.0f, 0.0f);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Enable face culling
    glEnable(GL_CULL_FACE);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window)) {
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui window
        {
            ImGui::Begin("ImGui Scenes", nullptr, ImGuiWindowFlags_MenuBar);

            // Display all scenes
            ImGui::BeginTabBar("SceneTabs", ImGuiTabBarFlags_None);
            // Scene 1 tab
            if (ImGui::BeginTabItem("Scene 1", nullptr, ImGuiTabItemFlags_None)) {
                ImGui::BeginChild("Scene1", ImVec2(0, 0), true);
                // Display Scene 1 content here
                // Draw the skybox
               // DrawSkybox(skybox, skyboxShderProgram, view, projection);

                // Draw objects for scene 1
                std::vector <glm::vec3> vec1 = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.2f, 0.2f, 0.2f) };
                drawObj(trans, road, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec1);

                std::vector <glm::vec3> vec4 = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(-225.0f, -255.0f, -2.0f),
                    glm::vec3(0.5f, 0.5f, 0.5f) };
                drawObj(trans, barn, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec4);

                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            // Scene 2 tab
            if (ImGui::BeginTabItem("Scene 2", nullptr, ImGuiTabItemFlags_None)) {
                ImGui::BeginChild("Scene2", ImVec2(0, 0), true);
                // Display Scene 2 content here
                // Draw the skybox
               // DrawSkybox(skybox, skyboxShderProgram, view, projection);

                // Draw objects for scene 2
                std::vector <glm::vec3> vec1 = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.2f, 0.2f, 0.2f) };
                drawObj(trans, road, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec1);

                std::vector <glm::vec3> vec5 = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(120.0f, 100.0f, -5.0f),
                    glm::vec3(0.03f, 0.03f, 0.03f) };
                drawObj(trans, structure, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec5);

                std::vector <glm::vec3> vec6 = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(90.0f, 100.0f, 34.0f),
                    glm::vec3(0.01f / 1.05, 0.01f / 1.05, 0.01f / 1.05) };
                drawObj(trans, structure2, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec6);

                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            // Scene 3 tab
            if (ImGui::BeginTabItem("Scene 3", nullptr, ImGuiTabItemFlags_None)) {
                ImGui::BeginChild("Scene3", ImVec2(0, 0), true);
                // Display Scene 3 content here
                // Draw the skybox
               // DrawSkybox(skybox, skyboxShderProgram, view, projection);

                // Draw objects for scene 3
                std::vector <glm::vec3> vec1 = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.2f, 0.2f, 0.2f) };
                drawObj(trans, road, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec1);

                std::vector <glm::vec3> vec6 = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(110.0f, 85.0f, -2.0f),
                    glm::vec3(0.03f, 0.03f, 0.03f) };
                drawObj(trans, structure3, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec6);

                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            // Scene 4 tab
            if (ImGui::BeginTabItem("Scene 4", nullptr, ImGuiTabItemFlags_None)) {
                ImGui::BeginChild("Scene4", ImVec2(0, 0), true);
                // Display Scene 4 content here
                // Draw the skybox
              //  DrawSkybox(skybox, skyboxShderProgram, view, projection);

                // Draw objects for scene 4
                std::vector <glm::vec3> vec1 = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.2f, 0.2f, 0.2f) };
                drawObj(trans, road, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec1);

                std::vector <glm::vec3> vec6 = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(90.0f, 100.0f, 2.0f),
                    glm::vec3(0.03f, 0.03f, 0.03f) };
                drawObj(trans, structure4, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec6);

                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            // Scene 5 tab
            if (ImGui::BeginTabItem("Scene 5", nullptr, ImGuiTabItemFlags_None)) {
                ImGui::BeginChild("Scene5", ImVec2(0, 0), true);
                // Display Scene 5 content here
                // Draw the skybox
              //  DrawSkybox(skybox, skyboxShderProgram, view, projection);

                // Draw objects for scene 5
                std::vector <glm::vec3> vec1 = { glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.2f, 0.2f, 0.2f) };
                drawObj(trans, road, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec1);

                std::vector <glm::vec3> vec2 = { glm::vec3(0.0f, 0.0f, 1.0f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.2f, 0.2f, 0.2f) };
                drawObj(trans, earth, shaderProgram, normalTransformLoc, modelTransformLoc,
                    0.0f, vec2);

                std::vector <glm::vec3> vec3 = { glm::vec3(0.0f, 0.0f, 1.0f),
                    glm::vec3(5.0f, 0.0f, 0.0f),
                    glm::vec3(0.03f, 0.03f, 0.03f) };
                drawObj(trans, moon, shaderProgram, normalTransformLoc, modelTransformLoc,
                    270.0f, vec3);

                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();

            ImGui::End();
        }

        // Rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void drawObj(glm::mat4& trans, ObjData& structure, GLuint& shaderProgram, GLuint& normalTransformLoc, GLuint& modelTransformLoc,
    float deg, std::vector<glm::vec3>& Vec3) // Vec3 = rotate axis, translation, scaling
{
    glBindVertexArray(structure.vaoId);
    glUseProgram(shaderProgram);

    // Calculate transformation matrix
    trans = glm::mat4(1.0f); // identity
    trans = glm::rotate(trans, glm::radians(deg), Vec3[0]);
    trans = glm::translate(trans, Vec3[1]);
    trans = glm::scale(trans, Vec3[2]);

    // Send transformation matrices to shader
    glm::mat4 normalTrans = glm::transpose(glm::inverse(trans));
    glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
    glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    GLuint structureTexture = structure.textures[structure.materials[0].diffuse_texname];
    glBindTexture(GL_TEXTURE_2D, structureTexture);

    // Draw object
    glDrawElements(GL_TRIANGLES, structure.numFaces, GL_UNSIGNED_INT, (void*)0);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}