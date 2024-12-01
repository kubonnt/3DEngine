#pragma once

#include <cstdint>
#include <unordered_map>
#include <memory>
#include "Component.h"

class Entity
{
public:
	using IdType = uint32_t;

	explicit Entity(IdType id) : id(id) {}

	IdType GetId() const { return id; }

	template <typename T>
	void AddComponent(std::shared_ptr<T> component)
	{
		components[typeid(T).hash_code()] = component;
	}

	template <typename T>
	T* GetComponent()
	{
		auto it = components.find(typeid(T).hash_code());
		if (it != components.end())
		{
			return static_cast<T*>(it->second.get());
		}
		return nullptr;
	}

private:
	IdType id;
	std::unordered_map<size_t, std::shared_ptr<Component>> components;
};