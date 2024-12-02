#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "../../ecs/include/System.h"
#include "MeshRendererComponent.h"
#include "ShaderComponent.h"
#include <vector>
#include <glm.hpp>
#include <gtx/string_cast.hpp>
#include <iostream>

class RenderSystem : public System
{
public:
	glm::mat4 globalViewMatrix;       // View matrix, should be set based on the camera
	glm::mat4 globalProjectionMatrix; // Projection matrix, should be set based on the perspective

	void Update(float deltaTime) override
	{
		for (const auto& entity : entities)
		{
			auto shaderComponent = entity.GetComponent<ShaderComponent>();
			auto meshRendererComponent = entity.GetComponent<MeshRendererComponent>();
			if (shaderComponent && meshRendererComponent) {
				shaderComponent->UseShader();

				// Set the required transformation matrices
				//glm::mat4 modelMatrix = glm::mat4(1.0f);  // Identity matrix for simplicity (or entity specific transform)
				shaderComponent->shader->SetMat4("model", modelMatrix);
				shaderComponent->shader->SetMat4("view", globalViewMatrix);
				shaderComponent->shader->SetMat4("projection", globalProjectionMatrix);
				

				// Render the mesh
				glBindVertexArray(meshRendererComponent->VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);  // Assuming we're drawing a cube
				//glBindVertexArray(0);
			}
		}
	}

	void OnComponentAdded(Entity& entity, std::shared_ptr<Component> component) 
	{
		if (std::dynamic_pointer_cast<ShaderComponent>(component) || std::dynamic_pointer_cast<MeshRendererComponent>(component)) 
		{
			// Only add entities that have both ShaderComponent and MeshRendererComponent
			if (entity.HasComponent<ShaderComponent>() && entity.HasComponent<MeshRendererComponent>())
			{
				if (std::find(entities.begin(), entities.end(), entity) == entities.end())
				{
					entities.push_back(entity);
					std::cout << "Entity with ID " << entity.GetID() << " added to RenderSystem." << std::endl;
				}
			}
		}
		
	}

	void AddEntity(Entity& entity) 
	{
		entities.push_back(entity);
		std::cout << "Entity with ID " << entity.GetID() << " added manually to RenderSystem." << std::endl;
	}

	glm::mat4 modelMatrix = glm::mat4(1.0f);

private:
	std::vector<Entity> entities;
};