#pragma once

#include "../../ecs/include/Component.h"
#include <glad/glad.h>  // or <glad/glad.h> if you're using GLAD

struct MeshRendererComponent : public Component {
  unsigned int VAO;

  MeshRendererComponent(unsigned int vao) : VAO(vao) {}
};