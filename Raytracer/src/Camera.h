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
    [[nodiscard]] void SetPosition(glm::vec3 pos) { m_Position = pos; }
    [[nodiscard]] glm::vec3 GetBackgroundColour() const { return m_BackgroundColour; }
    [[nodiscard]] void SetBackgroundColour(glm::vec4 colour) { m_BackgroundColour = colour; }
    [[nodiscard]] bool GetAntiAliasing() const { return m_AntiAliasing; }
    [[nodiscard]] float GetAntiAliasingAmount() const { return m_AntiAliasingAmount; }
    [[nodiscard]] void SetAntiAliasing(bool aa) { m_AntiAliasing = aa; }

  private:
    glm::vec3 m_Position;
    glm::vec3 m_BackgroundColour;

    bool m_AntiAliasing = false;
    float m_AntiAliasingAmount = 100;
  };
}
