#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace BackEnd
{
	void Init();
	void BeginFrame();
	void EndFrame();
	void CleanUpGLFW();

	void CreateGLFWWindow();
	GLFWwindow* GetWindowPtr();
	bool IsWindowOpen();
	int GetWindowWidth();
	int GetWindowHeight();
}