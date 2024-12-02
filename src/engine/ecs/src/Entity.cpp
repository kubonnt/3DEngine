#include "../include/Entity.h"
#include "../include/ECSManager.h"
#include "../../renderer/include/ShaderComponent.h"
#include "../../renderer/include/MeshRendererComponent.h"

void Entity::SetManager(ECSManager* ecsManager) {
  manager = ecsManager;
}

// Move AddComponent implementation here
template <typename T>
void Entity::AddComponent(std::shared_ptr<T> component) {
  size_t typeHash = std::type_index(typeid(T)).hash_code();
  components[typeHash] = component;

  if (manager) {
    manager->NotifyComponentAdded(*this,  std::static_pointer_cast<Component>(component));
  }
}

// Explicitly instantiate AddComponent for common components to allow compilation in the .cpp
template void Entity::AddComponent<ShaderComponent>(std::shared_ptr<ShaderComponent>);
template void Entity::AddComponent<MeshRendererComponent>(std::shared_ptr<MeshRendererComponent>);

// If you have more components, add explicit template instantiations here
