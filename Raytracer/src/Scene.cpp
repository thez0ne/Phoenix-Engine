#include "Scene.h"

#include <algorithm>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Raytracing
{
  Scene::Scene(uint16_t numberOfObjects, uint16_t numberOfMats)
  {
    m_Objects.reserve(numberOfObjects);
    m_Materials.reserve(numberOfMats);
  }

  Scene::~Scene()
  {
    // TODO cleanup objects that Scene took ownership of, or use smart pointers
    for (Hitable* obj : m_Objects)
    {
      delete obj;
    }
    for (Material* mat : m_Materials)
    {
      delete mat;
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

  void Scene::AddMaterial(Material* material)
  {
    m_Materials.push_back(material);
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
      obj->RenderOptions(counter++, GetVectorOfMatNames());
    }

    ImGui::Separator();
    // ImGui::Text("Materials");
    if (ImGui::CollapsingHeader("Materials"))
    {
      counter = 0;
      for (Material* mat : m_Materials)
      {
        ImGui::Separator();
        mat->RenderOptions(counter++);
      }
    }

    ImGui::End();
  }

  std::vector<std::string> Scene::GetVectorOfMatNames() const
  {
    std::vector<std::string> matNames;
    matNames.reserve(m_Materials.size());
    for (Material* mat : m_Materials)
    {
      matNames.push_back(mat->name);
    }
    return matNames;
  }
}
