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

    void ResetFrameCount() { m_FrameCount = 1; }
    [[nodiscard]] bool& GetAccumulateSetting() { return m_Accumulate; }

  private:
    Phoenix::Image* m_FinalImage;
    uint32_t* m_ImageData;
    uint32_t m_ViewportWidth, m_ViewportHeight;

    bool m_Accumulate = true;
    glm::vec4* m_AccumulatedData = nullptr;
    int m_FrameCount = 1;

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;

    glm::vec4 PixelColour(glm::vec2 coords);
  };
}
