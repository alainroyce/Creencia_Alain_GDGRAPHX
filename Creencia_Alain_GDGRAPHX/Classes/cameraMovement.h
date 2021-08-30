#ifndef CAMERA_MOVEMENT
#define CAMERA_MOVEMENT

#include <stdio.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
//#include "main.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class cameraMovement
{
public:
	cameraMovement();

	//camera
	static glm::vec3 cameraPos;
	static glm::vec3 cameraFront;
	static glm::vec3 cameraUp;
	static float yaw;
	static float pitch;
	static float fov;

	//mouse state
	static bool firstMouse;
	static float lastX;
	static float lastY;

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void moveCam(GLFWwindow* window, float deltaTime);

	static void initialize();
	static cameraMovement* getInstance();

private:
	void clampMovement();
	const std::vector <float> clampX = { -400.0f, 485.0f };
	const std::vector <float> clampY = { 0.0f, 6.0f };
	const std::vector <float> clampZ = { -450.0f, 450.0f };

private:
	static cameraMovement* sharedInstance;
};

#endif // !1

