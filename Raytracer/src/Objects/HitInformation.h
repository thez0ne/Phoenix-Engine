#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Raytracing
{
  struct HitInformation
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 colour;

    float t;
    float hitDistance = 999999999; // TODO infinity?
  };
}
