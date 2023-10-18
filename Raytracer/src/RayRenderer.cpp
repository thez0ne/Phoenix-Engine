#include "RayRenderer.h"

// #include <random>
#include "Utilities/Random.h"

namespace Raytracing
{
  RayRenderer::RayRenderer() : m_ViewportWidth(800), m_ViewportHeight(600)
  {
    m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
    for (size_t i = 0; i < m_ViewportWidth * m_ViewportHeight; i++)
    {
      m_ImageData[i] = Phoenix::Image::VecToRgba(glm::vec4(.1f, .4f, .1f, 1.f));
    }
    m_FinalImage =
      Phoenix::Image::Create(m_ViewportWidth, m_ViewportHeight, Phoenix::Format::RGBA, m_ImageData);
  }

  RayRenderer::~RayRenderer()
  {
    delete m_FinalImage;
    delete[] m_ImageData;
  }

  void RayRenderer::Render(const Scene& scene, const Camera& camera)
  {
    m_ActiveScene = &scene;
    m_ActiveCamera = &camera;

    for (size_t y = 0; y < m_ViewportHeight; y++)
    {
      for (size_t x = 0; x < m_ViewportWidth; x++)
      {
        glm::vec2 remappedCoords = {(float)x / (float)m_FinalImage->GetWidth(),
                                    (float)y / (float)m_FinalImage->GetHeight()};
        remappedCoords = remappedCoords * 2.0f - 1.0f;
        auto colour = PixelColour(remappedCoords);
        colour = glm::clamp(colour, glm::vec4(0.f), glm::vec4(1.f));
        m_ImageData[x + y * m_ViewportWidth] = Phoenix::Image::VecToRgba(colour);
      }
    }

    m_FinalImage->SetData(m_ImageData);
  }

  void RayRenderer::Resize()
  {
    if (m_FinalImage->GetWidth() != m_ViewportWidth ||
        m_FinalImage->GetHeight() != m_ViewportHeight)
    {
      m_FinalImage->Resize(m_ViewportWidth, m_ViewportHeight);
    }

    delete[] m_ImageData;
    m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
  }

  void RayRenderer::SaveImage(const std::string& fileName)
  {
    m_FinalImage->Save(fileName);
  }

  void RayRenderer::OnImGuiUpdate()
  {
    m_ViewportWidth = ImGui::GetContentRegionAvail().x;
    m_ViewportHeight = ImGui::GetContentRegionAvail().y;

    m_FinalImage->ImGuiBind();
  }

  glm::vec4 RayRenderer::PixelColour(glm::vec2 coords)
  {
    // TODO currently doesnt handle aspect ratio
    Ray ray;
    ray.origin = m_ActiveCamera->GetPosition();

    glm::vec3 accumulatedColor(0.0f);
    float multiplier = 1.0f;

    // TODO have this controlled by a user controlled option
    const int bounce = 2;

    // TODO fix AA
    ray.dir = glm::normalize(glm::vec3(coords.x, coords.y, -1.f));
    for (int i = 0; i < bounce; i++)
    {
      HitInformation hitInfo = m_ActiveScene->ShootRay(ray);
      if (hitInfo.hitDistance < 0.f)
      {
        accumulatedColor += m_ActiveCamera->GetBackgroundColour() * multiplier;
        break;
      }

      auto lightPos = m_ActiveScene->GetLightPos();
      float lightIntensity = glm::dot(lightPos - hitInfo.position, hitInfo.normal);

      glm::vec3 sphereColor = hitInfo.colour;
      sphereColor *= lightIntensity;
      accumulatedColor += sphereColor * multiplier;

      multiplier *= 0.7f;

      ray.origin = hitInfo.position + hitInfo.normal * 0.0001f;
      ray.dir = glm::reflect(ray.dir, hitInfo.normal);
    }

    return glm::vec4(accumulatedColor, 1.0f);

    // if (!scene.GetCamera()->GetAntiAliasing())
    // {
    //   // no AA
    //   ray.dir = glm::normalize(glm::vec3(coords.x, coords.y, -1.f));
    //   return scene.ShootRay(ray);
    // }

    // // with AA
    // glm::vec4 color(0.f);
    // auto AAIntensity = scene.GetCamera()->GetAntiAliasingAmount();

    // ray.dir = glm::normalize(glm::vec3(coords.x + Random::RandomDoubleRange(0.0, 0.001),
    //                                    coords.y + Random::RandomDoubleRange(0.0, 0.001),
    //                                    -1.f));
    // for (int i = 0; i < AAIntensity; i++)
    // {
    //   color += scene.ShootRay(ray);
    // }
    // return color / AAIntensity;
  }
}
