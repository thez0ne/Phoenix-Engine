#include "Scene.h"

#include <algorithm>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Raytracing
{
  Scene::Scene(uint16_t numberOfObjects)
  {
    m_Objects.reserve(numberOfObjects);
  }

  Scene::~Scene()
  {
    // TODO cleanup objects that Scene took ownership of, or use smart pointers
    for (Hitable* obj : m_Objects)
    {
      delete obj;
    }
  }

  HitInformation Scene::ShootRay(const Ray& ray) const
  {
    HitInformation closestHit;

    // TODO is there a better way to find the closest hit object?
    bool hasHit = false;

    for (Hitable* obj : m_Objects)
    {
      auto result = obj->Hit(ray);

      if (result)
      {
        auto hit = result.value();
        hasHit = true;
        if (hit.hitDistance < closestHit.hitDistance)
        {
          closestHit = result.value();
        }
      }
    }
    if (!hasHit)
    {
      closestHit.hitDistance = -1.f;
    }

    return closestHit;
  }

  void Scene::AddToScene(Hitable* object)
  {
    m_Objects.push_back(object);
  }

  void Scene::RenderHierarchy()
  {
    ImGui::Begin("Hierarchy");

    ImGui::Separator();

    ImGui::Text("Objects");
    ImGui::DragFloat3("Light Position", glm::value_ptr(m_LightPos));

    int counter = 0;
    for (Hitable* obj : m_Objects)
    {
      ImGui::Separator();
      obj->RenderOptions(counter++);
    }

    ImGui::End();
  }
}
