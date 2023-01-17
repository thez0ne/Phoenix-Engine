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

    for (size_t i = 0; i < m_ViewportWidth * m_ViewportHeight; i++)
    {
      m_ImageData[i] = Phoinix::Image::VecToRgba(m_BackgroundColour);
    }
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
}