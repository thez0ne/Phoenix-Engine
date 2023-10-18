#include "Sphere.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Raytracing
{
  Sphere::Sphere(const glm::vec3& position, float radius) : m_Position(position), m_Radius(radius)
  {}

  std::optional<HitInformation> Sphere::Hit(const Ray& ray)
  {
    auto a = glm::dot(ray.dir, ray.dir);
    auto b = 2.0 * glm::dot(ray.origin - m_Position, ray.dir);
    auto c = glm::dot(ray.origin - m_Position, ray.origin - m_Position) - m_Radius * m_Radius;

    auto discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0)
    {
      return std::nullopt;
    }

    // TODO figure out which hit actually is the closest point to camera
    float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    float t2 = (-b + glm::sqrt(discriminant)) / (2.0f * a);

    // PRINT("We have t1:{} and t2:{}", t1, t2);
    if (t1 < 0)
    {
      return std::nullopt;
    }

    // origin + dir * t -> coord of hit
    glm::vec3 h1 = ray.origin + ray.dir * t1;
    glm::vec3 h2 = ray.origin + ray.dir * t2;

    glm::vec3 normal = glm::normalize(h1 - m_Position);

    HitInformation result{};
    result.position = h1;
    result.normal = normal;
    result.materialIndex = m_MaterialIndex;
    result.t = t1;
    result.hitDistance = glm::distance(h1, ray.origin);

    return result;
  }

  void Sphere::RenderOptions(int id, const std::vector<std::string>& matList)
  {
    ImGui::PushID(id);
    ImGui::Text("Sphere");
    ImGui::DragFloat3("Position", glm::value_ptr(m_Position), 0.01f);
    ImGui::DragFloat("Radius", &m_Radius, 0.01f);

    if (ImGui::BeginCombo("Material", matList[m_MaterialIndex].c_str()))
    {
      for (int n = 0; n < matList.size(); n++)
      {
        const bool is_selected = (m_MaterialIndex == n);
        if (ImGui::Selectable(matList[n].c_str(), is_selected))
          m_MaterialIndex = n;

        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }

    ImGui::PopID();
  }
}
