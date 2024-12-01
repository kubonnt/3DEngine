#include "engine/ecs/include/ECSManager.h"
#include "engine/renderer/include/OpenGLRenderer.h"
#include "engine/renderer/include/MeshRenderer.h"
#include "engine/renderer/include/RenderSystem.h"
#include <iostream>

// Cube vertices
float vertices[] = {
  // Back face
  -0.5f, -0.5f, -0.5f,  // Bottom-left
   0.5f, -0.5f, -0.5f,  // Bottom-right
   0.5f,  0.5f, -0.5f,  // Top-right
   0.5f,  0.5f, -0.5f,  // Top-right
  -0.5f,  0.5f, -0.5f,  // Top-left
  -0.5f, -0.5f, -0.5f,  // Bottom-left

  // Front face
  -0.5f, -0.5f,  0.5f,  // Bottom-left
   0.5f, -0.5f,  0.5f,  // Bottom-right
   0.5f,  0.5f,  0.5f,  // Top-right
   0.5f,  0.5f,  0.5f,  // Top-right
  -0.5f,  0.5f,  0.5f,  // Top-left
  -0.5f, -0.5f,  0.5f,  // Bottom-left

  // Left face
  -0.5f,  0.5f,  0.5f,  // Top-right
  -0.5f,  0.5f, -0.5f,  // Top-left
  -0.5f, -0.5f, -0.5f,  // Bottom-left
  -0.5f, -0.5f, -0.5f,  // Bottom-left
  -0.5f, -0.5f,  0.5f,  // Bottom-right
  -0.5f,  0.5f,  0.5f,  // Top-right

  // Right face
   0.5f,  0.5f,  0.5f,  // Top-left
   0.5f,  0.5f, -0.5f,  // Top-right
   0.5f, -0.5f, -0.5f,  // Bottom-right
   0.5f, -0.5f, -0.5f,  // Bottom-right
   0.5f, -0.5f,  0.5f,  // Bottom-left
   0.5f,  0.5f,  0.5f,  // Top-left

   // Bottom face
   -0.5f, -0.5f, -0.5f,  // Top-left
    0.5f, -0.5f, -0.5f,  // Top-right
    0.5f, -0.5f,  0.5f,  // Bottom-right
    0.5f, -0.5f,  0.5f,  // Bottom-right
   -0.5f, -0.5f,  0.5f,  // Bottom-left
   -0.5f, -0.5f, -0.5f,  // Top-left

   // Top face
   -0.5f,  0.5f, -0.5f,  // Top-left
    0.5f,  0.5f, -0.5f,  // Top-right
    0.5f,  0.5f,  0.5f,  // Bottom-right
    0.5f,  0.5f,  0.5f,  // Bottom-right
   -0.5f,  0.5f,  0.5f,  // Bottom-left
   -0.5f,  0.5f, -0.5f   // Top-left
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialise GLFW" << std::endl;
		return -1;
	}

	// Create window and context 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	GLFWwindow* window = glfwCreateWindow(800, 600, "3D Game Engine", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	OpenGLRenderer renderer;
	renderer.InitializeImpl();

	ECSManager ecsManager;

	auto renderSystem = std::make_shared<RenderSystem>();
	ecsManager.AddSystem(renderSystem);

	Entity entity = ecsManager.CreateEntity();
	entity.AddComponent(std::make_shared<MeshRenderer>(renderer));

	while (!glfwWindowShouldClose(window))
	{
		ecsManager.UpdateSystems(0.016f);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	renderer.ShutdownImpl();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}