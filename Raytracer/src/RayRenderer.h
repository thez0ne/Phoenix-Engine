#pragma once

#include <Phoinix/Image.h>
#include <glm/vec4.hpp>

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

    glm::vec4 m_BackgroundColour = glm::vec4(1.0f, .0f, .0f, 1.0f);
  };
}