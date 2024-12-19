#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include "../mgl/mgl.hpp"

enum button
{
	PRESSED,
	NOT_PRESSED
};

class InputManager
{
public:
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void cursorCallback(GLFWwindow* window, double xpos, double ypos);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void windowSizeCallback(GLFWwindow* window, int width, int height);

	void setCamera(mgl::Camera *cam);
private:
	// Orthographic LeftRight(-2,2) BottomTop(-2,2) NearFar(1,10)
	glm::mat4 orthogonalProjection =
		glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 1.0f, 10.0f);
	// Perspective Fovy(30) Aspect(640/480) NearZ(1) FarZ(10)
	glm::mat4 perspectiveProjection =
		glm::perspective(glm::radians(30.0f), 640.0f / 480.0f, 1.0f, 1000.0f);
	mgl::Camera* camera = nullptr;

	//key states
	button leftMouse = NOT_PRESSED;

	//cursor variables
	static double cursorXPos, startCursorXPos;
	static double cursorYPos, startCursorYPos;

	//spherical coordinates variables
	float radius = 0, rotX = 0, rotY = 0;
};

