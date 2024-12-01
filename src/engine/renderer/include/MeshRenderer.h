#pragma once

#include "../../ecs/include/Component.h"
#include "../include/OpenGLRenderer.h"

struct MeshRenderer : public Component {
  MeshRenderer(OpenGLRenderer& renderer) : renderer(renderer) {}

  void Render() {
    renderer.RenderImpl();
  }

private:
  OpenGLRenderer& renderer;
};