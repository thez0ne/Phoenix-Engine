#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace Raytracing
{
  class Material
  {
  public:
    virtual void RenderOptions()
    {
      ImGui::DragFloat("Roughness", &m_Roughness, 0.001f, 0.f, 1.f);
      ImGui::DragFloat("Metallic", &m_Metallic, 0.001f, 0.f, 1.f);
      ImGui::ColorEdit4("Albedo", glm::value_ptr(m_Albedo));
    };

    glm::vec4 m_Albedo{1.0f};
    float m_Roughness = 1.f;
    float m_Metallic = 0.f;

  private:
  };
}