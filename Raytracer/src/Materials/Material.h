#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <string>

namespace Raytracing
{
  class Material
  {
  public:
    virtual void RenderOptions(int id)
    {
      ImGui::PushID(id);
      ImGui::Text(name.c_str());
      ImGui::DragFloat("Roughness", &Roughness, 0.001f, 0.f, 1.f);
      ImGui::DragFloat("Metallic", &Metallic, 0.001f, 0.f, 1.f);
      ImGui::ColorEdit3("Albedo", glm::value_ptr(Albedo));
      ImGui::PopID();
    };

    std::string name = "Material";

    glm::vec3 Albedo{1.0f};
    float Roughness = 1.f;
    float Metallic = 0.f;

  private:
  };
}