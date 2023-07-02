#pragma once

#include <glm/glm.hpp>

namespace Raytracing
{
  class Camera
  {
  public:
    Camera(const glm::vec3& position = glm::vec3(0.f),
           const glm::vec4& backgroundColour = glm::vec4(0.f));
    ~Camera() = default;

    void CameraSettings();

    [[nodiscard]] glm::vec3 GetPosition() const { return m_Position; }
    [[nodiscard]] glm::vec4 GetBackgroundColour() const { return m_BackgroundColour; }

  private:
    glm::vec3 m_Position;
    glm::vec4 m_BackgroundColour;
  };
}
