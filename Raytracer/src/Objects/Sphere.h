#pragma once

#include "Hitable.h"
#include <glm/glm.hpp>

namespace Raytracing
{
  class Sphere : public Hitable
  {
  public:
    Sphere(const glm::vec3& position = glm::vec3(0.f),
           float radius = 1.f,
           glm::vec4 colour = glm::vec4(1.f, 0.f, 0.f, 1.f));
    ~Sphere() = default;

    virtual std::optional<HitInformation> Hit(const Ray& ray) override;

  private:
    glm::vec3 m_Position;
    float m_Radius;

    glm::vec4 m_Colour;
  };
}
