#include "cameraMovement.h"

cameraMovement::cameraMovement()
{

}

void cameraMovement::initialize()
{
	sharedInstance = new cameraMovement();
}

cameraMovement* cameraMovement::sharedInstance = NULL;

cameraMovement* cameraMovement::getInstance()
{
	return sharedInstance;
}

void cameraMovement :: moveCam(GLFWwindow* window, float deltaTime)
{
	float cameraSpeed = deltaTime * 100;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraMovement::getInstance()->cameraPos += cameraSpeed * cameraMovement::getInstance()->cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraMovement::getInstance()->cameraPos -= cameraSpeed * cameraMovement::getInstance()->cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraMovement::getInstance()->cameraPos -= glm::normalize(
			glm::cross(cameraMovement::getInstance()->cameraFront,
				cameraMovement::getInstance()->cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraMovement::getInstance()->cameraPos += glm::normalize(glm::cross(
			cameraMovement::getInstance()->cameraFront,
			cameraMovement::getInstance()->cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		cameraMovement::getInstance()->cameraPos += cameraSpeed * cameraMovement::getInstance()->cameraUp;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraMovement::getInstance()->cameraPos -= cameraSpeed * cameraMovement::getInstance()->cameraUp;
	}
	cameraMovement::getInstance()->clampMovement();
}

void cameraMovement :: clampMovement()
{
	//std::cout << "X: " << cameraMovement::getInstance()->cameraPos.x << std::endl;
	//std::cout << "Y: " << cameraMovement::getInstance()->cameraPos.y << std::endl;
	//std::cout << "z: " << cameraMovement::getInstance()->cameraPos.z << std::endl;

	if (cameraMovement::getInstance()->cameraPos.x < this->clampX[0])
	{
		cameraMovement::getInstance()->cameraPos.x = this->clampX[0];
	}
	if (cameraMovement::getInstance()->cameraPos.x > this->clampX[1])
	{
		cameraMovement::getInstance()->cameraPos.x = this->clampX[1];
	}
	if (cameraMovement::getInstance()->cameraPos.y < this->clampY[0])
	{
		cameraMovement::getInstance()->cameraPos.y = this->clampY[0];
	}
	if (cameraMovement::getInstance()->cameraPos.y > this->clampY[1])
	{
		cameraMovement::getInstance()->cameraPos.y = this->clampY[1];
	}
	if (cameraMovement::getInstance()->cameraPos.z < this->clampZ[0])
	{
		cameraMovement::getInstance()->cameraPos.z = this->clampZ[0];
	}
	if (cameraMovement::getInstance()->cameraPos.z > this->clampZ[1])
	{
		cameraMovement::getInstance()->cameraPos.z = this->clampZ[1];
	}
}

//properties
glm::vec3 cameraMovement :: cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraMovement:: cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraMovement:: cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraMovement:: yaw = -90.0f;
float cameraMovement:: pitch = 0.0f;
float cameraMovement:: fov = 45.0f;

//mouse state
bool cameraMovement:: firstMouse = true;
float cameraMovement:: lastX = 1024 / 2.0;
float cameraMovement:: lastY = 768 / 2.0;

void cameraMovement :: mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
