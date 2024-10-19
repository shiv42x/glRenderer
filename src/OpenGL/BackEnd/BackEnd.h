#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../Input/InputMapper.h"

namespace BackEnd
{
	void Init();

	// window creation and handling
	void CreateGLFWWindow();
	GLFWwindow* GetWindowPtr();
	bool IsWindowOpen();
	int GetWindowWidth();
	int GetWindowHeight();

	// rendering
	void BeginFrame();
	void EndFrame();
	void CleanUpGLFW();

	// input handling
	bool ConvertGLFWKeyToRawButton(int key, InputMapping::RawInputButton& button);

	// callbacks
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
}