#include "../include/ECSManager.h"

Entity ECSManager::CreateEntity()
{
	return Entity(nextEntityId++);
}

void ECSManager::DestroyEntity(Entity entity)
{
	components.erase(entity.GetId());
}

template <typename T, typename... Args>
void ECSManager::AddComponent(Entity entity, Args&&... args)
{
	auto component = std::make_shared<T>(std::forward<Args>(args)...);
	components[entity.GetId()].push_back(component);
}

template <typename T>
T* ECSManager::GetComponent(Entity entity)
{
	auto& entityComponents = components[entity.GetId()];
	for (auto& comp : entityComponents)
	{
		if (T* casted = dynamic_cast<T*>(comp.get()))
		{
			return casted;
		}
	}
	return nullptr;
}

void ECSManager::AddSystem(std::shared_ptr<System> system)
{
	systems.push_back(system);
}

std::vector<std::shared_ptr<System>>& ECSManager::GetSystems() 
{
	return systems;
}

void ECSManager::UpdateSystems(float deltaTime)
{
	for (auto& system : systems)
	{
		system->Update(deltaTime);
	}
}