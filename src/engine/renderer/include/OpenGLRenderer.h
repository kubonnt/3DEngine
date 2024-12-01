#pragma once

#include "Renderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class OpenGLRenderer : public Renderer<OpenGLRenderer>
{
public:
	void InitializeImpl()
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cerr << "Failed to initialise GLAD" << std::endl;
			return;
		}
		glEnable(GL_DEPTH_TEST);
		std::cout << "OpenGL Renderer initialised." << std::endl;
	}

	void RenderImpl()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void ShutdownImpl()
	{
		std::cout << "OpenGL Renderer shutdown." << std::endl;
	}
};