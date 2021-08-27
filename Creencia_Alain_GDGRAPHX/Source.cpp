#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "obj_mesh.h";
#include "shader.h"
#include "skybox.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	window = glfwCreateWindow(1024, 768, "Creencia_Alain", NULL, NULL);
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

	ObjData backpack;
	LoadObjFile(&backpack, "earth/Earth.obj");
	GLfloat bunnyOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&backpack,
		1.0f,
		bunnyOffsets
	);

	ObjData tourusObjData;
	LoadObjFile(&tourusObjData, "earth/Earth.obj");
	GLfloat tourusOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&tourusObjData,
		1.0f,
		tourusOffsets
	);

	ObjData planetObjData;
	LoadObjFile(&planetObjData, "earth/Earth.obj");
	GLfloat planetOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&planetObjData,
		1.0f,
		planetOffsets
	);
	std::vector<std::string> faces
	{
		"right.png",
		"left.png",
		"bottom.png",
		"top.png",
		"front.png",
		"back.png"
	};
	SkyBoxData skybox = LoadSkybox("Assets/skybox", faces);
#pragma endregion

#pragma region Shader Loading
	GLuint skyboxShaderProgram = LoadShaders("Shaders/skybox_vertex.shader", "Shaders/skybox_fragment.shader");
	GLuint shaderProgram = LoadShaders("Shaders/phong_vertex.shader", "Shaders/phong_fragment.shader");
	//glUseProgram(shaderProgram2);
	//GLuint shaderProgram = LoadShaders("Shaders/vertex.shader", "Shaders/fragment.shader");
	glUseProgram(shaderProgram);

	GLuint colorLoc = glGetUniformLocation(shaderProgram, "u_color");
	glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

	
	
	// initialize MVP
	GLuint modelTransformLoc = glGetUniformLocation(shaderProgram, "u_model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "u_view");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "u_projection");

	GLuint normalTransformLoc = glGetUniformLocation(shaderProgram, "u_normal");
	GLuint cameraPosLoc = glGetUniformLocation(shaderProgram, "u_camera_pos");
	GLuint ambientColorLoc = glGetUniformLocation(shaderProgram, "u_ambient_color");
	glUniform3f(ambientColorLoc, 0.1f, 0.1f, 0.1f);

	glm::mat4 trans = glm::mat4(1.0f); // identity
	glm::mat4 trans2 = glm::mat4(1.0f); // identity
	glm::mat4 trans3 = glm::mat4(1.0f); // identity


	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans2));
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans3));

	// define projection matrix
	glm::mat4 projection = glm::mat4(1.0f);
	//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "u_light_pos");
	GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "u_light_dir");
	glUniform3f(lightPosLoc, -trans2[3][0], -trans2[3][1], -trans2[3][2] + 2);
	glUniform3f(lightDirLoc, 0.0f, 0.0f, 0.0f);


#pragma endregion

	// set bg color to green
	glClearColor(0.4f, 0.4f, 0.0f, 0.0f);

	// var for rotations
	float xFactor = 0.0f;
	float xSpeed = 1.0f;
	float currentTime = glfwGetTime();
	float prevTime = 0.0f;
	float deltaTime = 0.0f;
	float rotFactor = 0.0f;

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
		projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
		// Set projection matrix in shader
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			// Perspective Projection
			projection = glm::perspective(glm::radians(90.0f), ratio, 0.1f, 100.0f),
			// Set projection matrix in shader
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		}

			

#pragma endregion

#pragma region View
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -5.0f);
		glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 90.0f, -1.0f),
			//glm::vec3(0.5f, 0.0f, -1.0f),
			glm::vec3(trans[3][0], trans[3][1], trans[3][2]),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.y);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -5.0f);
			glm::mat4 view = glm::lookAt(
				glm::vec3(0.0f, 2.0f, -5.0f),
				//glm::vec3(0.5f, 0.0f, -1.0f),
				glm::vec3(trans[3][0], trans[3][1], trans[3][2]),
				glm::vec3(0.0f, 1.0f, 0.0f)
			);
			glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.y);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		}

#pragma endregion

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//toggle to render wit GL_FILL or GL_LINE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#pragma region Draw

		DrawSkybox(skybox, skyboxShaderProgram, view, projection);
		glUseProgram(shaderProgram);


		//draw bunny
		glBindVertexArray(backpack.vaoId);
		glUseProgram(shaderProgram); //changes
		//////////////////////////////////////////////////
		// transforms
		trans = glm::mat4(1.0f); // identity
		trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f)); // matrix * translate_matrix
		trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
		trans = glm::rotate(trans, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix

		//send to shader
		glm::mat4 normalTrans = glm::transpose(glm::inverse(trans));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		

		glActiveTexture(GL_TEXTURE0);
		GLuint backpackTexture = backpack.textures[backpack.materials[0].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, backpackTexture);
		//drawbackpack
		glDrawElements(GL_TRIANGLES, backpack.numFaces, GL_UNSIGNED_INT, (void*)0);
		
		////////////////////////////////////////////////////////////////////////////////////////////////////
			
		// transforms
		trans2 = glm::mat4(1.0f); // identity
		trans2 = glm::rotate(trans, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		trans2 = glm::translate(trans2, glm::vec3(-3.0f, 0.0f, -5.0f)); // matrix * translate_matrix
		trans2 = glm::scale(trans2, glm::vec3(0.4f, 0.4f, 0.4f));
		//send to shader
		glm::mat4 normalTrans2 = glm::transpose(glm::inverse(trans2));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans2));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans2));

		glActiveTexture(GL_TEXTURE0);
		GLuint earthTexture = tourusObjData.textures[tourusObjData.materials[1].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, earthTexture);
		//drawbackpack
		glDrawElements(GL_TRIANGLES, tourusObjData.numFaces, GL_UNSIGNED_INT, (void*)0);
		
		/////////////////////////
		trans3 = glm::mat4(1.0f); // identity
		trans3 = glm::rotate(trans2, glm::radians(rotFactor), glm::vec3(0.0f, 1.0f, 0.0f)); // matrix * rotation_matrix
		trans3 = glm::translate(trans3, glm::vec3(-0.5f, 0.0f, -5.0f)); // matrix * translate_matrix
		trans3 = glm::scale(trans3, glm::vec3(0.3f, 0.3f, 0.3f));
		//send to shader
		glm::mat4 normalTrans3 = glm::transpose(glm::inverse(trans3));
		glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans3));
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans3));

		glActiveTexture(GL_TEXTURE0);
		GLuint moonTexture = planetObjData.textures[planetObjData.materials[2].diffuse_texname];
		glBindTexture(GL_TEXTURE_2D, moonTexture);
		//drawbackpack
		glDrawElements(GL_TRIANGLES, planetObjData.numFaces, GL_UNSIGNED_INT, (void*)0);


		//unbindtexture after rendering
		glBindTexture(GL_TEXTURE_2D, 0);

		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		rotFactor += deltaTime * 20.0f;
		/*if (rotFactor > 360.0f) {
			rotFactor -= 360.0f;
		}*/
		prevTime = currentTime;

		// incerement rotation by deltaTime
		/*currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		xFactor += deltaTime * xSpeed;
		if (xFactor > 1.0f) {
			xSpeed = -1.0f;
		}
		else if (xFactor < -1.0f) {
			xSpeed = 1.0f;
		}
		prevTime = currentTime;*/


		//draw tourus
		glBindVertexArray(tourusObjData.vaoId);
		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		//glDrawElements(GL_TRIANGLES, tourusObjData.numFaces, GL_UNSIGNED_INT, (void*)0);


		//--- stop drawing here ---
#pragma endregion

		glfwSwapBuffers(window);
		//listen for glfw input events
		glfwPollEvents();
	}
	return 0;
}