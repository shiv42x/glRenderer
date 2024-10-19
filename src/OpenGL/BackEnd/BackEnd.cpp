#include "BackEnd.h"

#include <iostream>

namespace BackEnd
{
	GLFWwindow* window = NULL;
	int windowWidth = 800;
	int windowHeight = 600;

	InputMapping::InputMapper Mapper;

	void Init()
	{
		/* Initialize the library */
		if (!glfwInit())
		{
			std::cout << "Failed to initialize GLFW." << std::endl;
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		CreateGLFWWindow();
		glfwMakeContextCurrent(window);

		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetKeyCallback(window, key_callback);

		/* Initialize GLAD */
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD." << std::endl;
			glfwTerminate();
			return;
		}
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

	void BeginFrame()
	{
		glfwPollEvents();
		//Mapper.Dispatch();
		//Mapper.Clear();
	}

	void EndFrame()
	{
		glfwSwapBuffers(window);
	}

	void CleanUpGLFW()
	{
		glfwTerminate();
	}

	bool ConvertGLFWKeyToRawButton(int key, InputMapping::RawInputButton& button)
	{
		switch (key)
		{
		case GLFW_KEY_W:	button = InputMapping::RAW_BUTTON_ONE;      break;
		case GLFW_KEY_S:	button = InputMapping::RAW_BUTTON_TWO;      break;
		case GLFW_KEY_A:	button = InputMapping::RAW_BUTTON_THREE;	break;
		case GLFW_KEY_D:    button = InputMapping::RAW_BUTTON_FOUR;     break;
		default:            return false;                               break;
		}

		return true;
	}
	
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		InputMapping::RawInputButton button;
		if (!ConvertGLFWKeyToRawButton(key, button))
		{
			std::cout << "Key not mapped\n";
			return;
		}

		if (action == GLFW_PRESS)
		{
			Mapper.SetRawButtonState(button, true, false);
		}
		else if (action == GLFW_RELEASE)
		{
			Mapper.SetRawButtonState(button, false, true);
		}
		else if (action == GLFW_REPEAT)
		{
			Mapper.SetRawButtonState(button, true, true);
		}
	}
}