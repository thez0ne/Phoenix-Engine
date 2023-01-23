#pragma once

#include <Phoinix/Image.h>
#include <glm/glm.hpp>

#include "Ray.h"

#include <string>

namespace Raytracing
{
  class RayRenderer
  {
  public:
    RayRenderer();
    ~RayRenderer();

    void Render();
    void Resize();
    void SaveImage(const std::string& fileName);
    void OnImGuiUpdate();
    void OnBackgroundColourUpdate();

  private:
    Phoinix::Image* m_FinalImage;
    uint32_t* m_ImageData;
    uint32_t m_ViewportWidth, m_ViewportHeight;

    glm::vec4 m_BackgroundColour = glm::vec4(.3f, .2f, .5f, 1.0f);

    glm::vec4 PixelColour(glm::vec2 coords);
    void TraceRay(const Ray& ray);

    // Camera stuff
    glm::vec3 m_CameraCenter = glm::vec3(0.f, 0.f, 2.f);

    // Geometry stuff
    float m_Radius = 0.5f;
    glm::vec3 m_SpherePos = glm::vec3(.0f, 0.f, -1.f);
    glm::vec4 m_SphereColour = glm::vec4(1.f, 0.f, 0.f, 1.0f);

  public:
    // Light coming from
    glm::vec3 m_LightPos = glm::vec3(-1.f, -1.f, -2.f);
  };
}