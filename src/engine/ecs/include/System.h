#pragma once

#include <vector>
#include "Entity.h"

class System 
{
public:
  virtual void Update(float deltaTime) = 0;

  // Virtual method to notify systems when a component is added to an entity
  virtual void OnComponentAdded(Entity& entity, std::shared_ptr<Component> component) {
    // By default, do nothing. Derived systems can override this.
  }

  virtual ~System() = default;

protected:
  std::vector<Entity> entities;
};