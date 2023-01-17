#include "RayRenderer.h"

namespace Raytracing
{
  RayRenderer::RayRenderer() : m_ViewportWidth(800), m_ViewportHeight(600)
  {
    m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
    for (size_t i = 0; i < m_ViewportWidth * m_ViewportHeight; i++)
    {
      m_ImageData[i] = Phoinix::Image::VecToRgba(m_BackgroundColour);
    }
    m_FinalImage =
      Phoinix::Image::Create(m_ViewportWidth, m_ViewportHeight, Phoinix::Format::RGBA, m_ImageData);
  }

  RayRenderer::~RayRenderer()
  {
    delete m_FinalImage;
    delete[] m_ImageData;
  }

  void RayRenderer::Render()
  {
    for (size_t y = 0; y < m_ViewportHeight; y++)
    {
      for (size_t x = 0; x < m_ViewportWidth; x++)
      {
        glm::vec2 remappedCoords = {(float)x / (float)m_FinalImage->GetWidth(),
                                    (float)y / (float)m_FinalImage->GetHeight()};
        remappedCoords = remappedCoords * 2.0f - 1.0f;
        auto colour = PixelColour(remappedCoords);
        m_ImageData[x + y * m_ViewportWidth] = Phoinix::Image::VecToRgba(colour);
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

  void RayRenderer::OnBackgroundColourUpdate()
  {
    ImGui::ColorEdit4("Background Colour", (float*)&m_BackgroundColour);
  }

  glm::vec4 RayRenderer::PixelColour(glm::vec2 coords)
  {
    // TODO currently doesnt handle aspect ratio
    Ray ray;
    ray.origin = m_CameraCenter;
    ray.dir = glm::normalize(glm::vec3(coords.x, coords.y, -1.f));

    auto a = glm::dot(ray.dir, ray.dir);
    auto b = 2.0 * dot(ray.origin - m_SpherePos, ray.dir);
    auto c = dot(ray.origin - m_SpherePos, ray.origin - m_SpherePos) - m_Radius * m_Radius;

    auto discriminant = b * b - 4.0f * a * c;
    if (discriminant >= 0)
      return m_SphereColour;

    return m_BackgroundColour;
  }

  void RayRenderer::TraceRay(const Ray& ray) {}
}