#pragma once

#include <vector>
#include "Entity.h"

class System 
{
public:
  virtual void Update(float deltaTime) = 0;

  void AddEntity(Entity entity) {
    entities.push_back(entity);
  }

protected:
  std::vector<Entity> entities;
};