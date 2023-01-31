#pragma once

#include "Objects/Hitable.h"
#include "Ray.h"
#include <vector>

#include <glm/glm.hpp>

namespace Raytracing
{
  class Scene
  {
  public:
    Scene(uint16_t numberOfObjects = 5);
    ~Scene();

    [[nodiscard]] glm::vec4 ShootRay(const Ray& ray) const;
    void AddToScene(Hitable* object);

    // TODO add function to remove from scene
    // TODO add imgui functionality to interact with each shape that exists

    void RenderHierarchy();

  private:
    std::vector<Hitable*> m_Objects;
    // TODO support a vector of lights
    glm::vec3 m_LightPos = glm::vec3(-3.f, 1.f, 1.f);
  };
}
