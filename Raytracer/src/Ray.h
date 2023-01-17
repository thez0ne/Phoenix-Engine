#pragma once

#include <glm/vec3.hpp>

namespace Raytracing
{
  struct Ray
  {
    glm::vec3 origin;
    glm::vec3 dir;
  };
}