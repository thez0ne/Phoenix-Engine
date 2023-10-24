#pragma once

#include "Objects/Hitable.h"
#include "Ray.h"
#include "Camera.h"
#include <vector>

#include <glm/glm.hpp>

namespace Raytracing
{
  class Scene
  {
  public:
    Scene(uint16_t numberOfObjects = 5, uint16_t numberOfMats = 5);
    ~Scene();

    [[nodiscard]] HitInformation ShootRay(const Ray& ray) const;
    void AddToScene(Hitable* object);
    void AddMaterial(Material* material);

    // TODO add function to remove from scene

    void RenderHierarchy();

    [[nodiscard]] const glm::vec3& GetLightPos() const { return m_LightPos; }
    [[nodiscard]] const Material& GetMaterial(int index) const { return *m_Materials[index]; }

  private:
    std::vector<Hitable*> m_Objects;
    std::vector<Material*> m_Materials;
    // TODO support a vector of lights
    glm::vec3 m_LightPos = glm::vec3(-3.f, 1.f, 1.f);

    std::vector<std::string> GetVectorOfMatNames() const;
  };
}
