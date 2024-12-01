#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include "Entity.h"
#include "Component.h"
#include "System.h"

class ECSManager
{
public:
	Entity CreateEntity();
	void DestroyEntity(Entity entity);
	
	template <typename T, typename... Args>
	void AddComponent(Entity entity, Args&&... args);

	template <typename T>
	T* GetComponent(Entity entity);

	void AddSystem(std::shared_ptr<System> system);
	std::vector<std::shared_ptr<System>>& GetSystems();
	void UpdateSystems(float deltaTime);

private:
	Entity::IdType nextEntityId = 0;
	std::unordered_map<Entity::IdType, std::vector<std::shared_ptr<Component>>> components;
	std::vector<std::shared_ptr<System>> systems;
};