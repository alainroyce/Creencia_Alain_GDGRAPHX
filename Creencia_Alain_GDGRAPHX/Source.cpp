#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Classes/cameraMovement.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "obj_mesh.h";
#include "shader.h"
#include "skybox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


//method declarations
void drawObj(glm::mat4& trans, ObjData& structure, GLuint& shaderProgram, GLuint& normalTransformLoc, GLuint& modelTransformLoc,
	float deg, std::vector<glm::vec3>& Vec3); //Vec3 = rotate axis, translation, scaling


enum SceneState {
	SCENE_1,
	SCENE_2,
	SCENE_3,
	SCENE_4,
	SCENE_5,
	SCENE_6
};

SceneState currentScene = SCENE_1;


int main() {
	stbi_set_flip_vertically_on_load(true);
#pragma region Initialization
	//initialize glfw
	if (glfwInit() != GLFW_TRUE) {
		fprintf(stderr, "Failed to initialized! \n");
		return -1;
	}

	// set opengl version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window 
	GLFWwindow* window;
	window = glfwCreateWindow(1024, 768, "Parcm Creencia-Villegas", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to load window! \n");
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set GLFW input mode
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//initialize glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

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
	glClearColor(0.4f, 0.4f, 0.0f, 0.0f);

	// var for rotations
	float xFactor1 = 0.0f, xFactor2 = 0.0f, xFactor3 = 0.0f;
	float xSpeed1 = 40.0f, xSpeed2 = 40.0f, xSpeed3 = 20.0f;
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
		ImGui::End();
		

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
		// incerement rotation by deltaTime
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;

		glm::mat4 view = glm::lookAt(cameraMovement::getInstance()->cameraPos,
			cameraMovement::getInstance()->cameraPos + cameraMovement::getInstance()->cameraFront,
			cameraMovement::getInstance()->cameraUp);

		//w,a,s,d movement of the camera
		cameraMovement::getInstance()->moveCam(window, deltaTime);

		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans1));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
#pragma endregion

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		//toggle to render wit GL_FILL or GL_LINE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#pragma region Draw
		//------------ start drawing here

		//deltaTime value
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		


		//Grass
		std::vector <glm::vec3> vec2 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(40.0f, 0.0f, -15.0f), //translate values
			glm::vec3(3.0f, 3.0f, 1.0f) }; //scaling values
		drawObj(trans1, earth, shaderProgram, normalTransformLoc, modelTransformLoc,
			270.0f, vec2);

		//MOON
		std::vector <glm::vec3> vec3 = { glm::vec3(0.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(0.0f, 30.0f, 0.0f), //translate values
			glm::vec3(0.5f, 0.5f, 0.5f) }; //scaling values
		drawObj(trans, moon, shaderProgram, normalTransformLoc, modelTransformLoc,
			0.0f, vec3);
		if (currentScene == SCENE_1) {
			//Road
			std::vector <glm::vec3> vec1 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
				glm::vec3(0.0f, 0.0f, 0.0f), //translate values
				glm::vec3(0.2f, 0.2f, 0.2f) }; //scaling values
			drawObj(trans1, road, shaderProgram, normalTransformLoc, modelTransformLoc,
				270.0f, vec1);
			//Powerplant
			std::vector <glm::vec3> vec4 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
				glm::vec3(-225.0f, -255.0f, -2.0f), //translate values
				glm::vec3(0.5f, 0.5f, 0.5f) }; //scaling values
			drawObj(trans1, barn, shaderProgram, normalTransformLoc, modelTransformLoc,
				270.0f, vec4);
		}
		else if (currentScene == SCENE_2) {
			//Road
			std::vector <glm::vec3> vec1 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
				glm::vec3(0.0f, 0.0f, 0.0f), //translate values
				glm::vec3(0.2f, 0.2f, 0.2f) }; //scaling values
			drawObj(trans1, road, shaderProgram, normalTransformLoc, modelTransformLoc,
				270.0f, vec1);
			//Church
			std::vector <glm::vec3> vec5 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
				glm::vec3(120.0f, 100.0f, -5.0f), //translate values
				glm::vec3(0.03f, 0.03f, 0.03f) }; //scaling values
			drawObj(trans1, structure, shaderProgram, normalTransformLoc, modelTransformLoc,
				270.0f, vec5);

			//BUILDING
			std::vector <glm::vec3> vec6 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
				glm::vec3(90.0f, 100.0f, 34.0f), //translate values
				glm::vec3(0.01f / 1.05, 0.01f / 1.05, 0.01f / 1.05) }; //scaling values
			drawObj(trans1, structure2, shaderProgram, normalTransformLoc, modelTransformLoc,
				270.0f, vec6);
		}
		else if (currentScene == SCENE_3) {
			//Road
			std::vector <glm::vec3> vec1 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
				glm::vec3(0.0f, 0.0f, 0.0f), //translate values
				glm::vec3(0.2f, 0.2f, 0.2f) }; //scaling values
			drawObj(trans1, road, shaderProgram, normalTransformLoc, modelTransformLoc,
				270.0f, vec1);
			//Stadium
			std::vector <glm::vec3> vec8 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
				glm::vec3(150.0f, -290.0f, 0.0f), //translate values
				glm::vec3(0.001f * 6, 0.001f * 6, 0.001f * 6) }; //scaling values
			drawObj(trans1, structure4, shaderProgram, normalTransformLoc, modelTransformLoc,
				270.0f, vec8);
			//Grocery
			std::vector <glm::vec3> vec7 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
				glm::vec3(-120.0f, 85.0f, -5.0f), //translate values
				glm::vec3(0.016f, 0.016f, 0.016f) }; //scaling values
			drawObj(trans1, structure3, shaderProgram, normalTransformLoc, modelTransformLoc,
				270.0f, vec7);
		}
		else if (currentScene == SCENE_4) {
			//Road
			std::vector <glm::vec3> vec1 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
				glm::vec3(0.0f, 0.0f, 0.0f), //translate values
				glm::vec3(0.2f, 0.2f, 0.2f) }; //scaling values
			drawObj(trans1, road, shaderProgram, normalTransformLoc, modelTransformLoc,
				270.0f, vec1);


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

void drawObj(glm::mat4& trans, ObjData& structure, GLuint& shaderProgram, GLuint& normalTransformLoc, GLuint& modelTransformLoc,
	float deg, std::vector<glm::vec3>& Vec3) //Vec3 = rotate axis, translation, scaling
{
	glBindVertexArray(structure.vaoId);
	glUseProgram(shaderProgram); //changes

	// transforms
	trans = glm::mat4(1.0f); // identity
	trans = glm::rotate(trans, glm::radians(deg), Vec3[0]);
	trans = glm::translate(trans, Vec3[1]); // matrix * translate_matrix
	trans = glm::scale(trans, Vec3[2]);

	//send to shader
	glm::mat4 normalTrans = glm::transpose(glm::inverse(trans)); //changes
	glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans)); //changes
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	glActiveTexture(GL_TEXTURE0);
	GLuint structureTexture = structure.textures[structure.materials[0].diffuse_texname];
	glBindTexture(GL_TEXTURE_2D, structureTexture);

	//draw earth
	glDrawElements(GL_TRIANGLES, structure.numFaces, GL_UNSIGNED_INT, (void*)0);

	//unbindtexture after rendering
	glBindTexture(GL_TEXTURE_2D, 0);
}