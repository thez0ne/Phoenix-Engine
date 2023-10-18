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
    virtual void RenderOptions(int id, const std::vector<std::string>& matList) override;

    // [[nodiscard]] Material& GetMaterial() { return m_Material; }
    [[nodiscard]] void SetMaterialIndex(int index) { m_MaterialIndex = index; }

  private:
    glm::vec3 m_Position;
    float m_Radius;

    // Material m_Material;
    int m_MaterialIndex;
  };
}
