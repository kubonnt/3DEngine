#include "../include/ECSManager.h"

Entity ECSManager::CreateEntity() 
{
	Entity entity(nextEntityId++);  // Create a new entity with a unique ID
	entity.SetManager(this);
	entities.push_back(entity);     // Store the entity in the internal list
	return entity;
}

//void ECSManager::DestroyEntity(Entity entity)
//{
//	components.erase(entity.GetID());
//}

// Add a component to an entity
//template <typename T>
//void ECSManager::AddComponent(Entity& entity, std::shared_ptr<T> component) 
//{
//	components[std::type_index(typeid(T))][entity.GetID()] = component;

	// Register entity with relevant systems based on component type
//	for (auto& system : systems) 
//	{
//		if (auto renderSystem = std::dynamic_pointer_cast<RenderSystem>(system))
//		{
			// RenderSystem requires ShaderComponent and MeshRendererComponent
//			if (entity.HasComponent<ShaderComponent>() && entity.HasComponent<MeshRendererComponent>()) 
//			{
//				renderSystem->AddEntity(entity);
//			}
//		}
//	}
//}

template <typename T>
T* ECSManager::GetComponent(Entity entity)
{
	auto& entityComponents = components[entity.GetID()];
	for (auto& comp : entityComponents)
	{
		if (T* casted = dynamic_cast<T*>(comp.get()))
		{
			return casted;
		}
	}
	return nullptr;
}

//template <typename T>
//void ECSManager::AddSystem(std::shared_ptr<T> system)
//{
//	systems.push_back(system);
//}

//template <typename T>
//void ECSManager::NotifyComponentAdded(Entity& entity, std::shared_ptr<T> component) 
//{
//	for (auto& system : systems)
//	{
//		system->OnComponentAdded(entity, component);
//	}
// }

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