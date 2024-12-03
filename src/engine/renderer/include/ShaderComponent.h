#pragma once

#include "../../ecs/include/Component.h"
#include "Shader.h"
#include <memory>

struct ShaderComponent : public Component
{
  std::shared_ptr<Shader> shader;

  ShaderComponent(const std::string& vertexPath, const std::string& fragmentPath, const std::string& binaryPath)
  {
    shader = std::make_shared<Shader>(vertexPath, fragmentPath, binaryPath);
  }

  void UseShader()
  {
    shader->Use();
  }
};