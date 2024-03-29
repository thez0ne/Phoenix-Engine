#include "RayRenderer.h"

// #include <random>
#include "Utilities/Random.h"
#include "Utilities/Math.h"

namespace Raytracing
{
  RayRenderer::RayRenderer() : m_ViewportWidth(800), m_ViewportHeight(600)
  {
    m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
    m_AccumulatedData = new glm::vec4[m_ViewportWidth * m_ViewportHeight];
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
    delete[] m_AccumulatedData;
  }

  void RayRenderer::Render(const Scene& scene, const Camera& camera)
  {
    m_ActiveScene = &scene;
    m_ActiveCamera = &camera;

    if (m_FrameCount == 1)
    {
      memset(m_AccumulatedData, 0, m_ViewportWidth * m_ViewportHeight * sizeof(glm::vec4));
    }

    for (size_t y = 0; y < m_ViewportHeight; y++)
    {
      for (size_t x = 0; x < m_ViewportWidth; x++)
      {
        glm::vec2 remappedCoords = {(float)x / (float)m_FinalImage->GetWidth(),
                                    (float)y / (float)m_FinalImage->GetHeight()};
        remappedCoords = remappedCoords * 2.0f - 1.0f;
        auto colour = PixelColour(remappedCoords);

        m_AccumulatedData[x + y * m_ViewportWidth] += colour;

        glm::vec4 accumulatedColour = m_AccumulatedData[x + y * m_ViewportWidth];
        accumulatedColour /= (float)m_FrameCount;

        accumulatedColour = glm::clamp(accumulatedColour, glm::vec4(0.0f), glm::vec4(1.0f));
        m_ImageData[x + y * m_ViewportWidth] = Phoenix::Image::VecToRgba(accumulatedColour);
      }
    }

    m_FinalImage->SetData(m_ImageData);

    if (m_Accumulate)
    {
      m_FrameCount++;
    }
    else
    {
      m_FrameCount = 1;
    }
  }

  void RayRenderer::Resize()
  {
    if (m_FinalImage->GetWidth() == m_ViewportWidth &&
        m_FinalImage->GetHeight() == m_ViewportHeight)
    {
      return;
    }
    m_FinalImage->Resize(m_ViewportWidth, m_ViewportHeight);

    delete[] m_ImageData;
    m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];

    delete[] m_AccumulatedData;
    m_AccumulatedData = new glm::vec4[m_ViewportWidth * m_ViewportHeight];
    ResetFrameCount();
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
    ray.dir = glm::normalize(glm::vec3(coords.x, coords.y, -1.f));

    glm::vec3 accumulatedColor(0.0f);
    float multiplier = 1.0f;

    // TODO have this controlled by a user controlled option
    const int bounce = 2;

    // TODO fix AA
    auto AAAmount = m_ActiveCamera->GetAntiAliasingAmount();
    if (m_ActiveCamera->GetAntiAliasing())
    {
      ray.dir = glm::normalize(glm::vec3(coords.x + Random::RandomFloat() / 1000.f,
                                         coords.y + Random::RandomFloat() / 1000.f,
                                         -1.f));
    }

    for (int i = 0; i < bounce; i++)
    {
      HitInformation hitInfo = m_ActiveScene->ShootRay(ray);
      if (hitInfo.hitDistance < 0.f)
      {
        accumulatedColor += m_ActiveCamera->GetBackgroundColour() * multiplier;
        break;
      }

      auto lightPos = m_ActiveScene->GetLightPos();
      float lightIntensity =
        glm::max(glm::dot(glm::normalize(lightPos - hitInfo.position), hitInfo.normal), 0.f);
      // temp directional light solution
      // glm::vec3 lightDir = glm::normalize(glm::vec3(1, -1, -1));
      // float lightIntensity = glm::max(glm::dot(-lightDir, hitInfo.normal), 0.f);

      auto mat = m_ActiveScene->GetMaterial(hitInfo.materialIndex);

      // remapped from 0-1, to 0-0.8
      auto metallicInfluence = 1 - Math::remap(mat.Metallic, 0.f, 1.f, 0.f, .8f);
      glm::vec3 sphereColor = mat.Albedo * metallicInfluence;

      sphereColor *= lightIntensity;
      accumulatedColor += sphereColor * multiplier;

      // multiplier *= 0.5f;
      // remapped from 0-1, to 0-0.5
      auto remapped = 0.5f + Math::remap(.5f, 0.f, 1.f, 0.f, 0.5f) * mat.Metallic;
      multiplier *= remapped;

      // making sure bouncedray does not collide with the same surface
      ray.origin = hitInfo.position + hitInfo.normal * 0.0001f;

      auto randomDeflection = Random::RandomVector(-0.5, 0.5);
      auto isProperDir = glm::dot(randomDeflection, hitInfo.normal) < 0.f;
      ray.dir = glm::reflect(
        ray.dir,
        hitInfo.normal + mat.Roughness * (isProperDir ? randomDeflection : -randomDeflection));
    }

    return glm::vec4(accumulatedColor, 1.0f);
  }
}
