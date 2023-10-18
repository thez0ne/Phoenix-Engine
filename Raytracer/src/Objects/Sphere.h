#pragma once
#include "pch.h"
#include "Hitable.h"
#include "Materials/Material.h"
#include <glm/glm.hpp>

namespace Raytracing
{
  class Sphere : public Hitable
  {
  public:
    Sphere(const glm::vec3& position = glm::vec3(0.f), float radius = 1.f);
    ~Sphere() = default;

    virtual std::optional<HitInformation> Hit(const Ray& ray) override;
    virtual void RenderOptions(int id) override;

    [[nodiscard]] Material& GetMaterial() { return m_Material; }

  private:
    glm::vec3 m_Position;
    float m_Radius;

    Material m_Material;
  };
}
