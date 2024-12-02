#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include "Entity.h"
#include "System.h"
#include "Component.h"
#include "../../renderer/include/RenderSystem.h"

class ECSManager
{
public:
	Entity CreateEntity();
	//void DestroyEntity(Entity entity);
	

	void AddComponent(Entity& entity, std::shared_ptr<Component> component)
	{
		entity.AddComponent(component);
		for (auto& system : systems) {
			system->OnComponentAdded(entity, component);
			std::cout << "Component added to Entity ID: " << entity.GetID() << std::endl;
		}
	}

	template <typename T>
	T* GetComponent(Entity entity);

	template <typename T>
	void AddSystem(std::shared_ptr<T> system)
	{
		systems.push_back(system);
	}

	void NotifyComponentAdded(Entity& entity, std::shared_ptr<Component> component)
	{
		for (auto& system : systems)
		{
			system->OnComponentAdded(entity, component);
		}
	}

	std::vector<std::shared_ptr<System>>& GetSystems();
	void UpdateSystems(float deltaTime);

private:
	std::vector<Entity> entities;
	std::vector<std::shared_ptr<System>> systems;
	std::unordered_map<std::type_index, std::unordered_map<int, std::shared_ptr<Component>>> components;
	int nextEntityId = 0;
};