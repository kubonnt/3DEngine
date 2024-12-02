#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "engine/ecs/include/ECSManager.h"
#include "engine/renderer/include/OpenGLRenderer.h"
#include "engine/renderer/include/MeshRenderer.h"
#include "engine/renderer/include/MeshRendererComponent.h"
#include "engine/renderer/include/ShaderComponent.h"
#include "engine/renderer/include/RenderSystem.h"
#include <iostream>

// Set up the cube vertices (6 faces, 2 triangles per face, 3 vertices per triangle)
float vertices[] = {
  // Back face
  -0.5f, -0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   0.5f,  0.5f, -0.5f,
   0.5f,  0.5f, -0.5f,
  -0.5f,  0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,

  // Front face
  -0.5f, -0.5f,  0.5f,
   0.5f, -0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,
  -0.5f, -0.5f,  0.5f,

  // Left face
  -0.5f,  0.5f,  0.5f,
  -0.5f,  0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f, -0.5f,
  -0.5f, -0.5f,  0.5f,
  -0.5f,  0.5f,  0.5f,

  // Right face
   0.5f,  0.5f,  0.5f,
   0.5f,  0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   0.5f, -0.5f, -0.5f,
   0.5f, -0.5f,  0.5f,
   0.5f,  0.5f,  0.5f,

   // Bottom face
   -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
   -0.5f, -0.5f,  0.5f,
   -0.5f, -0.5f, -0.5f,

   // Top face
   -0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f,  0.5f,
   -0.5f,  0.5f, -0.5f,
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
	renderer.Initialize();

  // Check the actual OpenGL version and profile
  const GLubyte* GLrenderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  std::cout << "Renderer: " << GLrenderer << std::endl;
  std::cout << "OpenGL version supported: " << version << std::endl;

  int flags;
  glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &flags);
  if (flags & GL_CONTEXT_CORE_PROFILE_BIT) {
    std::cout << "Core profile is being used." << std::endl;
  }
  else {
    std::cout << "Compatibility profile is being used." << std::endl;
  }

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

	ECSManager ecsManager;
  Entity entity = ecsManager.CreateEntity();
  
  auto shaderComponent = std::make_shared<ShaderComponent>(
    "D:/Dev/3DEngine/src/shaders/vertex_shader.glsl",
    "D:/Dev/3DEngine/src/shaders/fragment_shader.glsl",
    "D:/Dev/3DEngine/src/shaders/shader_cache.bin"
  );
  entity.AddComponent(shaderComponent);

  unsigned int VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  //shaderComponent->UseShader();

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0); // Unbind VAO

  auto meshRendererComponent = std::make_shared<MeshRendererComponent>(VAO);
  entity.AddComponent(meshRendererComponent);
  //entity.AddComponent(std::make_shared<MeshRenderer>(renderer));

  auto renderSystem = std::make_shared<RenderSystem>();
  renderSystem->AddEntity(entity);

  // Set global matrices for Render System
  renderSystem->globalViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
  renderSystem->globalProjectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  //std::cout << "Model Matrix: " << glm::to_string(renderSystem->modelMatrix) << std::endl;
  //std::cout << "View Matrix: " << glm::to_string(renderSystem->globalViewMatrix) << std::endl;
  //std::cout << "Projection Matrix: " << glm::to_string(renderSystem->globalProjectionMatrix) << std::endl;
  ecsManager.AddSystem(renderSystem);

	while (!glfwWindowShouldClose(window))
	{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ecsManager.UpdateSystems(0.016f);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

  // renderer.Shutdown();
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}