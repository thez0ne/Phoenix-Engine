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

    delete m_Camera;
  }

  glm::vec4 Scene::ShootRay(const Ray& ray) const
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
      return m_Camera->GetBackgroundColour();
    }

    float lightIntensity = glm::dot(m_LightPos - closestHit.position, closestHit.normal);
    return closestHit.colour * lightIntensity;
  }

  void Scene::AddToScene(Hitable* object)
  {
    m_Objects.push_back(object);
  }

  void Scene::AddCameraToScene(Camera* camera)
  {
    m_Camera = camera;
  }

  void Scene::RenderHierarchy()
  {
    ImGui::Begin("Hierarchy");
    // TODO add objects

    ImGui::Separator();

    m_Camera->CameraSettings();

    ImGui::Separator();

    ImGui::Text("Objects");
    ImGui::DragFloat3("Light Position", glm::value_ptr(m_LightPos));

    ImGui::End();
  }
}
