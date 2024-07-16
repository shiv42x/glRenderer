#include "BackEnd.h"
#include <iostream>

namespace BackEnd
{
	GLFWwindow* window = NULL;
	int windowWidth = 800;
	int windowHeight = 600;

	void Init()
	{
		/* Initialize the library */
		if (!glfwInit())
		{
			std::cout << "Failed to initialize GLFW." << std::endl;
			return;
		}

		CreateGLFWWindow();
		glfwMakeContextCurrent(window);

		/* Initialize GLAD */
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD." << std::endl;
			return;
		}
	}

	void CleanUpGLFW()
	{
		glfwTerminate();
	}

	void BeginFrame()
	{
		glfwPollEvents();
	}

	void EndFrame()
	{
		glfwSwapBuffers(window);
	}

	void CreateGLFWWindow()
	{
		/* Create a windowed mode window and its OpenGL context */
		window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", NULL, NULL);
		if (!window)
		{
			std::cout << "Failed to create GLFW window." << std::endl;
			glfwTerminate();
			return;
		}
	}

	GLFWwindow* GetWindowPtr()
	{
		return window;
	}

	bool IsWindowOpen()
	{
		return (!glfwWindowShouldClose(window));
	}

	int GetWindowWidth()
	{
		return windowWidth;
	}

	int GetWindowHeight()
	{
		return windowHeight;
	}
}