//testtest
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
//#include "main.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "obj_mesh.h";
#include "shader.h"
#include "skybox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

//mouse state
bool firstMouse = true;
float lastX = 1024 / 2.0;
float lastY = 768 / 2.0;


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float mouseOffsetX = xpos - lastX;
	float mouseOffsetY = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	mouseOffsetX *= sensitivity;
	mouseOffsetY *= sensitivity;

	yaw += mouseOffsetX;
	pitch += mouseOffsetY;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}



	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}
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
	window = glfwCreateWindow(1024, 768, "Celestial, Emerson", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to load window! \n");
		return -1;
	}
	glfwMakeContextCurrent(window);

	//initialize glew
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
#pragma endregion


#pragma region Mesh Loading

	ObjData earth;
	//backpack.textures = 
	LoadObjFile(&earth, "grass/10438_Circular_Grass_Patch_v1_iterations-2.obj");
	GLfloat earthOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&earth,
		1.0f,
		earthOffsets
	);

	/*ObjData moon;
	//backpack.textures = 
	LoadObjFile(&moon, "earth/Earth.obj");
	GLfloat moonOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&moon,
		1.0f,
		moonOffsets
	);

	ObjData sun;
	//backpack.textures = 
	LoadObjFile(&sun, "earth/Earth.obj");
	GLfloat sunOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&sun,
		1.0f,
		sunOffsets
	);*/

	//Load skybox model; changes below
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

	GLuint shaderProgram = LoadShaders("Shaders/phong_vertex.shader", "Shaders/phong_fragment.shader"); //changes
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

	// define projection matrix
	glm::mat4 projection = glm::mat4(1.0f);
	//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//setup light shading; position of the flash light; changes
	GLuint lightPoscLoc = glGetUniformLocation(shaderProgram, "u_light_pos"); //changes
	//glUniform3f(lightPoscLoc, 0.0f, 1.0f, 0.0f);
	GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "u_light_dir"); //changes
	glUniform3f(lightPoscLoc, trans1[3][0], trans1[3][1]+1.0f, trans1[3][1]); //changes
	glUniform3f(lightDirLoc, 1.0f, 1.0f, 1.0f); //changes
	//glUniform3f(lightDirLoc, 0.0f, 1.0f, 0.0f);

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


	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//depth testing
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS); // set the depth test function

	//face culling
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // set which face to cull
	//glFrontFace(GL_CCW); // set the front face orientation

	while (!glfwWindowShouldClose(window)) {

#pragma region Viewport
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
#pragma endregion

#pragma region Projection
		// Orthopgraphic projection but make units same as pixels. origin is lower left of window
		// projection = glm::ortho(0.0f, (GLfloat)width, 0.0f, (GLfloat)height, 0.1f, 10.0f); // when using this scale objects really high at pixel unity size

		// Orthographic with stretching
		//projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 10.0f);

		// Orthographic with corection for stretching, resize window to see difference with previous example
		//projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 10.0f);

		// Perspective Projection
		//change to perspective view
		projection = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 10000.0f);
		// Set projection matrix in shader
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

#pragma endregion

#pragma region View
		// incerement rotation by deltaTime
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;

		/*glm::mat4 view;
		//camera position
		glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, -200.0f); //changes
		// side look
		view = glm::lookAt(
			cameraPos,
			//glm::vec3(0.5f, 0.0f, -1.0f),
			glm::vec3(trans1[3][0], trans1[3][1], trans1[3][2]),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.y); //changes
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));*/
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		float cameraSpeed = deltaTime/100;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			cameraPos += cameraSpeed * cameraFront;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			cameraPos -= cameraSpeed * cameraFront;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			cameraPos += cameraSpeed * cameraUp;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			cameraPos -= cameraSpeed * cameraUp;
		}
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans1));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
#pragma endregion

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//toggle to render wit GL_FILL or GL_LINE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#pragma region Draw

		DrawSkybox(skybox, skyboxShderProgram, view, projection); //changes

		/*////////// SUN
		glBindVertexArray(sun.vaoId);
		glUseProgram(shaderProgram); //changes

		// transforms
		trans2 = glm::mat4(1.0f); // identity
		//trans2 = glm::translate(trans2, glm::vec3(-0.8f, 0.0f, -3.0f)); // matrix * translate_matrix
		trans2 = glm::scale(trans2, glm::vec3(0.7f, 0.7f, 0.7f));
		//rotates itself; no revolution to other planets
		trans2 = glm::rotate(trans2, glm::radians(xFactor3), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		//send to shader
		glm::mat4 normalTrans2 = glm::transpose(glm::inverse(trans2)); //changes
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans2)); //changes
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans2));

		glActiveTexture(GL_TEXTURE0);
		GLuint sunTexture = sun.textures[sun.materials[2].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, sunTexture);

		// incerement rotation by deltaTime
		xFactor3 += deltaTime * xSpeed3;

		//draw sun
		glDrawElements(GL_TRIANGLES, sun.numFaces, GL_UNSIGNED_INT, (void*)0);

		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);
		*/

		//////////////EARTH
		glBindVertexArray(earth.vaoId);
		glUseProgram(shaderProgram); //changes

		// transforms
		trans1 = glm::mat4(1.0f); // identity
		//rotates it to the origin point in the world where the sun is also positioned; revolves around the Sun
		//trans1 = glm::rotate(trans1, glm::radians(xFactor2), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		trans1 = glm::rotate(trans, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		//trans1 = glm::rotate(trans, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		trans1 = glm::translate(trans1, glm::vec3(0.0f, 0.0f, 0.0f)); // matrix * translate_matrix
		trans1 = glm::scale(trans1, glm::vec3(1.0f, 1.0f, 1.0f));

		//send to shader
		glm::mat4 normalTrans1 = glm::transpose(glm::inverse(trans1)); //changes
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans1)); //changes
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans1));

		glActiveTexture(GL_TEXTURE0);
		GLuint earthTexture = earth.textures[earth.materials[0].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, earthTexture);

		// incerement rotation by deltaTime
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		xFactor2 += deltaTime * xSpeed2;

		//draw earth
		glDrawElements(GL_TRIANGLES, earth.numFaces, GL_UNSIGNED_INT, (void*)0);

		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);

		/*////////// MOON
		glBindVertexArray(moon.vaoId);
		glUseProgram(shaderProgram); //changes

		// transforms
		trans = glm::mat4(1.0f); // identity
		//rotates it to the origin point of the Earth; revolves around the Earth
		trans = glm::rotate(trans1, glm::radians(xFactor1), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		trans = glm::translate(trans, glm::vec3(3.6f, 0.0f, -3.0f)); // matrix * translate_matrix
		trans = glm::scale(trans, glm::vec3(0.4f, 0.4f, 0.4f));
		//send to shader
		glm::mat4 normalTrans = glm::transpose(glm::inverse(trans2)); //changes
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans)); //changes
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glActiveTexture(GL_TEXTURE0);
		GLuint moonTexture = moon.textures[moon.materials[0].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, moonTexture);

		//directional light will also rotate
		//std::cout << xFactor1 << std::endl;
		//glUniform3f(lightPoscLoc, glm::radians(xFactor1 * 1.0), 0.0f, 0.0f); //changes

		// incerement rotation by deltaTime
		xFactor1 += deltaTime * xSpeed1;
		prevTime = currentTime;

		//draw moon
		glDrawElements(GL_TRIANGLES, moon.numFaces, GL_UNSIGNED_INT, (void*)0);

		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);*/

		//--- stop drawing here ---
#pragma endregion

		glfwSwapBuffers(window);
		//listen for glfw input events
		glfwPollEvents();
	}
	return 0;
}