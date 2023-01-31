#pragma once

#include <Phoinix/Image.h>
#include <glm/glm.hpp>

#include "Ray.h"
#include "Scene.h"

#include <string>

namespace Raytracing
{
  class RayRenderer
  {
  public:
    RayRenderer();
    ~RayRenderer();

    void Render(const Scene& scene);
    void Resize();
    void SaveImage(const std::string& fileName);
    void OnImGuiUpdate();
    void OnBackgroundColourUpdate();

  private:
    Phoinix::Image* m_FinalImage;
    uint32_t* m_ImageData;
    uint32_t m_ViewportWidth, m_ViewportHeight;

    glm::vec4 PixelColour(glm::vec2 coords, const Scene& scene);

    // Camera stuff
    glm::vec4 m_BackgroundColour = glm::vec4(.3f, .2f, .5f, 1.0f);
    glm::vec3 m_CameraCenter = glm::vec3(0.f, 0.f, 2.f);
  };
}
