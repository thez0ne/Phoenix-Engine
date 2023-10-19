#pragma once

#include <Phoenix/Image.h>
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

    void Render(const Scene& scene, const Camera& camera);
    void Resize();
    void SaveImage(const std::string& fileName);
    void OnImGuiUpdate();

  private:
    Phoenix::Image* m_FinalImage;
    uint32_t* m_ImageData;
    uint32_t m_ViewportWidth, m_ViewportHeight;

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;

    glm::vec4 PixelColour(glm::vec2 coords);
  };
}
