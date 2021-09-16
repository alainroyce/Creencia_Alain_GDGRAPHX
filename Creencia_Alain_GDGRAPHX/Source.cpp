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

//method declarations
void drawObj(glm::mat4& trans, ObjData& structure, GLuint& shaderProgram, GLuint& normalTransformLoc, GLuint& modelTransformLoc,
	float deg, float deg2, std::vector<glm::vec3>& Vec3, int type); //Vec3 = rotate axis, translation, scaling; deg2 for y rotation

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
	window = glfwCreateWindow(1024, 768, "Machine Project: Creencia and Celestial", NULL, NULL);
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
	LoadObjFile(&earth, "grass2/10450_Rectangular_Grass_Patch_v1_iterations-2.obj");
	GLfloat earthOffsets[] = { 0.0f, 0, 0 }; //x,z,y
	LoadObjToMemory(
		&earth,
		1.0f,
		earthOffsets
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
	ObjData trees;
	LoadObjFile(&trees, "Trees/trees9.obj");
	GLfloat treesOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&trees,
		1.0f,
		treesOffsets
	);
	ObjData oldHouse;
	LoadObjFile(&oldHouse, "CottageHouse/cottage_obj.obj");
	GLfloat oldHouseOffsets[] = { 0.0f, 0.0f, 0.0f };
	LoadObjToMemory(
		&oldHouse,
		1.0f,
		oldHouseOffsets
	);

	std::vector<std::string> faces{
		"right.png",
		"left.png",
		"bottom.png",
		"top.png",
		"front.png",
		"back.png"
	};
	std::vector<std::string> faces1{
		"right.png",
		"left.png",
		"bottom.png",
		"top.png",
		"front.png",
		"back.png"
	};
	std::vector<std::string> faces2{
		"right.png",
		"left.png",
		"bottom.png",
		"top.png",
		"front.png",
		"back.png"
	};

	SkyBoxData skyboxAfternoon = LoadSkybox("Assets/skybox", faces); //changes;
	SkyBoxData skyboxMorning = LoadSkybox("Assets/SkyboxDay", faces1); //changes
	SkyBoxData skyboxNightTime = LoadSkybox("Assets/SkyboxNight", faces2); //changes
#pragma endregion

#pragma region Shader Loading

	//LoadSkybox shader
	GLuint skyboxShderProgram = LoadShaders("Shaders/skybox_vertex.shader", "Shaders/skybox_fragment.shader"); //changes

	//used directional light for light source
	GLuint shaderProgram = LoadShaders("Shaders/phong_vertex.shader", "Shaders/phong_directional_fragment.shader"); //changes
	//GLuint shaderProgram = LoadShaders("Shaders/phong_vertex.shader", "Shaders/alpha_test_fragment.shader"); //changesMC
	//GLuint shaderProgram = LoadShaders("Shaders/phong_vertex.shader", "Shaders/earth_night_fragment.shader"); //changesMC; normal fragment
	glUseProgram(shaderProgram); //relevance

	GLuint colorLoc = glGetUniformLocation(shaderProgram, "u_color"); //relevance
	glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); //relevance

	// initialize MVP
	GLuint modelTransformLoc = glGetUniformLocation(shaderProgram, "u_model"); //relevance
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "u_view"); //relevance
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "u_projection"); //relevance

	//initialize normal transformation; changes
	GLuint normalTransformLoc = glGetUniformLocation(shaderProgram, "u_normal"); //changes //relevance
	GLuint model_id = glGetUniformLocation(shaderProgram, "model_id"); //newChange //relevance
	GLuint cameraPosLoc = glGetUniformLocation(shaderProgram, "u_camera_pos"); //changes //relevance
	GLuint ambientColorLoc = glGetUniformLocation(shaderProgram, "u_ambient_color"); //changes //relevance
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
	glm::mat4 trans8 = glm::mat4(1.0f); // identity
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans8)); //trees
	glm::mat4 trans9 = glm::mat4(1.0f); // identity
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans9)); //trees

	// define projection matrix
	glm::mat4 projection = glm::mat4(1.0f);
	//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//setup light shading; position of the flash light; changes
	GLuint lightPoscLoc = glGetUniformLocation(shaderProgram, "u_light_pos"); //changes //relevance
	//glUniform3f(lightPoscLoc, 0.0f, 1.0f, 0.0f);
	GLuint lightDirLoc = glGetUniformLocation(shaderProgram, "u_light_dir"); //changes //relevance 
	GLuint secondaryTexLoc = glGetUniformLocation(shaderProgram, "secondary_diffuse"); //changesMC; for secondary map //relevance
	GLuint diffuseTexLoc = glGetUniformLocation(shaderProgram, "texture_diffuse"); //changes1 //relevance
	GLuint normalTexLoc = glGetUniformLocation(shaderProgram, "texture_normal"); //changes1 //relevance


	//set textures; mapping the texture unit; only used for "earth_night_fragment.shader"
	glUniform1i(diffuseTexLoc, 0); //changesMC; used for setting the first png in the shader
	glUniform1i(secondaryTexLoc, 5); //changesMC; used for accessing the secondary png in the shader

	/* //for normal map
	glUniform1i(diffuseTexLoc, 0);//changes1
	glUniform1i(normalTexLoc, 1);//changes1
	*/

	glUniform3f(lightPoscLoc, 0.0f, 2.0f, 0.0f); //changes //x,z,y
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//depth testing
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS); // set the depth test function

	//face culling
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK); // set which face to cull
	//glFrontFace(GL_CCW); // set the front face orientation

	float currentX = 0.0f; //changesMC
	float currentY = 0.0f; //changesMC
	float timer = 0.0f; //changesMC

	while (!glfwWindowShouldClose(window)) {
		
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

		//balance na lang
		currentX += (deltaTime * 7.0f / 60.0f); //changesMC
		currentY += (deltaTime * 7.0f / 60.0f); //changesMC
		//0 - 8.0f
		timer += deltaTime;

		//directional light - set a new direction
		glUniform3f(lightDirLoc, glm::sin(currentX), glm::cos(currentY), 0); //changesMC
		//std::cout << timer << std::endl; //changesMC

		glm::mat4 view = glm::lookAt(cameraMovement::getInstance()->cameraPos,
			cameraMovement::getInstance()->cameraPos + cameraMovement::getInstance()->cameraFront,
			cameraMovement::getInstance()->cameraUp);

		//w,a,s,d movement of the camera
		cameraMovement::getInstance()->moveCam(window, deltaTime);

		glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans1));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
#pragma endregion

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//toggle to render wit GL_FILL or GL_LINE
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#pragma region Draw
		//------------ start drawing here
		
		//deltaTime value
		currentTime = glfwGetTime();
		deltaTime = currentTime - prevTime;
		prevTime = currentTime;

		//set to direct light
		glUniform1i(model_id, 1);

		//Skybox display
		if (timer <= 40.0)
		{
			DrawSkybox(skyboxNightTime, skyboxShderProgram, view, projection); //changes
		}
		//Skybox display
		else if (timer > 40.0f && timer <= 55.0f)
		{
			DrawSkybox(skyboxMorning, skyboxShderProgram, view, projection); //changes
		}
		//Skybox display
		else if (timer > 55.0f && timer <= 70.0f)
		{
			DrawSkybox(skyboxAfternoon, skyboxShderProgram, view, projection); //changes
		}
		if (timer >= 70.0f)
		{
			//go back to night
			timer = 10.0f;
			DrawSkybox(skyboxNightTime, skyboxShderProgram, view, projection); //changes
		}

		/*//changes3
		if(time_ticks)
		{
			DrawSkybox(skybox, skyboxShderProgram, view, projection); //changes
		}
		*/

		//Road
		std::vector <glm::vec3> vec1 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(0.0f, 0.0f, 0.0f), //translate values
			glm::vec3(0.2f, 0.2f, 0.2f) }; //scaling values
		drawObj(trans1, road, shaderProgram, normalTransformLoc, modelTransformLoc,
			270.0f, 0, vec1, 1);

		//set to multitext shader
		glUniform1i(model_id, 4);
		glUniform1i(diffuseTexLoc, 0); //changesMC; used for setting the first png in the shader
		glUniform1i(normalTexLoc, 1); //changesMC; used for accessing the secondary png in the shader
		//Cottage
		std::vector <glm::vec3> vec10 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(250.0, -7.5f, 0.0f), //translate values
			glm::vec3(2.0f, 2.0f, 2.0f), //scaling values
			glm::vec3(0.0f, 1.0f, 0.0f) }; //rotation for y axis
		drawObj(trans9, oldHouse, shaderProgram, normalTransformLoc, modelTransformLoc, //relevance
			0.0f, 90.0f, vec10, 3);

		//set to multitext shader
		glUniform1i(model_id, 2);
		glUniform1i(diffuseTexLoc, 1); //changesMC; used for setting the first png in the shader
		glUniform1i(secondaryTexLoc, 7); //changesMC; used for accessing the secondary png in the shader
		//Trees
		std::vector <glm::vec3> vec9 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(290.0, -5.0f, 0.0f), //translate values
			glm::vec3(2.0f, 2.0f, 2.0f), //scaling values
			glm::vec3(0.0f, 1.0f, 0.0f)}; //rotation for y axis
		drawObj(trans8, trees, shaderProgram, normalTransformLoc, modelTransformLoc, //relevance
			0.0f, 90.0f, vec9, 2);

		//back to 0 material
		glUniform1i(diffuseTexLoc, 0); //changesMC; used for setting the first png in the shader
		//bring back to direct light
		glUniform1i(model_id, 1);

		//Grass
		std::vector <glm::vec3> vec2 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(40.0f, 0.0f, -15.0f), //translate values
			glm::vec3(3.0f, 3.0f, 1.0f) }; //scaling values
		drawObj(trans7, earth, shaderProgram, normalTransformLoc, modelTransformLoc,
			270.0f, 0.0f, vec2, 1);

		//Powerplant
		std::vector <glm::vec3> vec4 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(-225.0f, -255.0f, -2.0f), //translate values
			glm::vec3(0.5f, 0.5f, 0.5f) }; //scaling values
		drawObj(trans2, barn, shaderProgram, normalTransformLoc, modelTransformLoc,
			270.0f, 0.0f, vec4, 1);

		glUniform1i(model_id, 3);
		glUniform3f(lightPoscLoc, 0, 2, 2); //changes //x,z,y
		//Church
		std::vector <glm::vec3> vec5 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(120.0f, 100.0f, -5.0f), //translate values
			glm::vec3(0.03f, 0.03f, 0.03f) }; //scaling values
		drawObj(trans3, structure, shaderProgram, normalTransformLoc, modelTransformLoc,
			270.0f, 0.0f, vec5, 1);
		glUniform3f(lightPoscLoc, 0, 2, 2); //changes //x,z,y

		//Grocery
		std::vector <glm::vec3> vec7 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(-120.0f, 85.0f, -5.0f), //translate values
			glm::vec3(0.016f, 0.016f, 0.016f) }; //scaling values
		drawObj(trans5, structure3, shaderProgram, normalTransformLoc, modelTransformLoc,
			270.0f, 0.0f, vec7, 1);

		//bring back to direct light
		glUniform1i(model_id, 1);
		glUniform3f(lightPoscLoc, 0, 2, 0); //changes //x,z,y
		glUniform3f(lightPoscLoc, 0.0f, 2.0f, 0.0f); //changes //x,z,y

		//Stadium
		std::vector <glm::vec3> vec8 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(150.0f, -290.0f, 0.0f), //translate values
			glm::vec3(0.001f * 6, 0.001f * 6, 0.001f * 6) }; //scaling values
		drawObj(trans6, structure4, shaderProgram, normalTransformLoc, modelTransformLoc,
			270.0f, 0.0f, vec8, 1);

		//spotlight
		glUniform1i(model_id, 5);
		glUniform3f(lightPoscLoc, 0, 0, 0); //changes //x,z,y
		glUniform3f(lightDirLoc, 1.0f, 0.0f, 0.0f); //changes
		//BUILDING
		std::vector <glm::vec3> vec6 = { glm::vec3(1.0f, 0.0f, 0.0f), //camera axis
			glm::vec3(90.0f, 100.0f, 34.0f), //translate values
			glm::vec3(0.01f / 1.05, 0.01f / 1.05, 0.01f / 1.05) }; //scaling values
		drawObj(trans4, structure2, shaderProgram, normalTransformLoc, modelTransformLoc,
			270.0f, 0.0f, vec6, 1);

		//bring back to direct light
		glUniform1i(model_id, 1);
		glUniform3f(lightPoscLoc, 0, 2, 0); //changes //x,z,y
		glUniform3f(lightPoscLoc, 0.0f, 2.0f, 0.0f); //changes //x,z,y

		//--- stop drawing here ---
#pragma endregion

		glfwSwapBuffers(window);
		//listen for glfw input events
		glfwPollEvents();
	}
	return 0;
}
/*
glm::mat4 trans6 = glm::mat4(1.0f); // identity
glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans6));
*/
void drawObj(glm::mat4 &trans, ObjData &structure, GLuint &shaderProgram, GLuint& normalTransformLoc, GLuint& modelTransformLoc,
	float deg, float deg2, std::vector<glm::vec3>& Vec3, int type) //Vec3 = rotate axis, translation, scaling; deg2 for y rotation
{ //1 is normal, 2 is multiTex, 3 is normal
	glBindVertexArray(structure.vaoId);
	glUseProgram(shaderProgram); //changes

	// transforms
	trans = glm::mat4(1.0f); // identity
	trans = glm::rotate(trans, glm::radians(deg), Vec3[0]);
	trans = glm::translate(trans, Vec3[1]); // matrix * translate_matrix
	trans = glm::scale(trans, Vec3[2]);
	//if greater than 4, then it has multitexturing components
	if (type == 2)
	{
		trans = glm::rotate(trans, glm::radians(deg2), Vec3[3]);
	}

	//send to shader
	glm::mat4 normalTrans = glm::transpose(glm::inverse(trans)); //changes
	glUniformMatrix4fv(normalTransformLoc, 1, GL_FALSE, glm::value_ptr(normalTrans)); //changes
	glUniformMatrix4fv(modelTransformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	glActiveTexture(GL_TEXTURE0);
	GLuint structureTexture = structure.textures[structure.materials[0].diffuse_texname];
	glBindTexture(GL_TEXTURE_2D, structureTexture);

	if (type == 2)
	{
		//this is for multitexturing //changesMC
		glActiveTexture(GL_TEXTURE0);
		GLuint structureTexture = structure.textures[structure.materials[1].diffuse_texname]; //morning light //changesMC
		glBindTexture(GL_TEXTURE_2D, structureTexture);
		

		//std::cout << "Multi-texturing applied" << std::endl; //changesMC
		//this is for the secondary map
		glActiveTexture(GL_TEXTURE1);//changesMC
		GLuint secondaryTex = structure.textures[structure.materials[7].diffuse_texname]; //nightLight //changesMC
		glBindTexture(GL_TEXTURE_2D, secondaryTex);//changesMC
	}
	if (type ==3)
	{
		//this is for multitexturing //changesMC
		glActiveTexture(GL_TEXTURE0);
		GLuint structureTexture = structure.textures[structure.materials[0].diffuse_texname]; //morning light //changesMC
		glBindTexture(GL_TEXTURE_2D, structureTexture);


		//std::cout << "Multi-texturing applied" << std::endl; //changesMC
		//this is for the secondary map
		glActiveTexture(GL_TEXTURE1);//changesMC
		GLuint secondaryTex = structure.textures[structure.materials[0].bump_texname]; //nightLight //changesMC
		glBindTexture(GL_TEXTURE_2D, secondaryTex);//changesMC
	}

	//draw earth
	glDrawElements(GL_TRIANGLES, structure.numFaces, GL_UNSIGNED_INT, (void*)0);

	/* //Use for blending
	glEnable(GL_BLEND); //changesMC
	//glDisable(GL_DEPTH_TEST);//changesMC //objects behind can be seen
	glBlendFunc(GL_SRC0_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //changesMC
	//draw moon
	glDrawElements(GL_TRIANGLES, moon.numFaces, GL_UNSIGNED_INT, (void*)0);
	//glEnable(GL_DEPTH_TEST);//changesMC
	glDisable(GL_BLEND); //changesMC
	*/

	//unbindtexture after rendering
	glBindTexture(GL_TEXTURE_2D, 0);
}