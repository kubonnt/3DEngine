#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cstdint>
#include "Component.h"

class ECSManager;

class Entity
{
public:
	explicit Entity(int id) : id(id), manager(nullptr) {}

	int GetID() const { return id; }

	template <typename T>
	bool HasComponent() const 
	{
		auto it = components.find(std::type_index(typeid(T)).hash_code());
		return it != components.end();
	}

	template <typename T>
	void AddComponent(std::shared_ptr<T> component);

	// Define equality operator based on the unique ID of the entity
	bool operator==(const Entity& other) const {
		return this->id == other.id;
	}

	template <typename T>
	std::shared_ptr<T> GetComponent() const 
	{
		auto it = components.find(std::type_index(typeid(T)).hash_code());
		if (it != components.end()) {
			return std::static_pointer_cast<T>(it->second);
		}
		return nullptr;
	}

	void SetManager(ECSManager* ecsManager);

private:
	int id;
	std::unordered_map<size_t, std::shared_ptr<Component>> components;
	ECSManager* manager = nullptr;
};