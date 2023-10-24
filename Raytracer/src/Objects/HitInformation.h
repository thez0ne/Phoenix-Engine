#pragma once

#include "Materials/Material.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Raytracing
{
  struct HitInformation
  {
    glm::vec3 position;
    glm::vec3 normal;

    float t;
    float hitDistance = std::numeric_limits<double>::infinity();

    // Material material;
    int materialIndex;
  };
}
