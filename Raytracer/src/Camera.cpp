#include "Camera.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Raytracing
{
  Camera::Camera(const glm::vec3& position, const glm::vec4& backgroundColour) :
    m_Position(position), m_BackgroundColour(backgroundColour)
  {}

  void Camera::CameraSettings()
  {
    ImGui::Begin("Camera");

    ImGui::DragFloat3("Position", glm::value_ptr(m_Position));
    ImGui::ColorEdit4("Background Colour", glm::value_ptr(m_BackgroundColour));

    ImGui::End();
  }
}
