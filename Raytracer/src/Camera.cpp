#include "Camera.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Raytracing
{
  Camera::Camera(const glm::vec3& position, const glm::vec4& backgroundColour) :
    m_Position(position), m_BackgroundColour(backgroundColour)
  {}
  // TODO create a viewport per camera?

  void Camera::CameraSettings()
  {
    ImGui::Text("Camera Settings");
    ImGui::DragFloat3("Position", glm::value_ptr(m_Position), 0.1f);
    ImGui::ColorEdit4("Background Colour", glm::value_ptr(m_BackgroundColour));
    ImGui::Checkbox("AntiAliasing", &m_AntiAliasing);
    ImGui::DragFloat("AntiAliasing Intensity", &m_AntiAliasingAmount);
  }
}
