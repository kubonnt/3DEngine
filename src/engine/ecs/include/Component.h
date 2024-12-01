#pragma once

#include <cstdint>

struct Component 
{
  using IdType = uint32_t;
  virtual ~Component() = default;
};