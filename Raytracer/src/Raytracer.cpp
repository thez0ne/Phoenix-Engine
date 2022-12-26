#include <Phoinix.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

class RaytracerLayer : public Phoinix::Layer
{
 public:
   RaytracerLayer() : Layer("Sandbox Layer"), m_ViewportWidth(800), m_ViewportHeight(600)
   {
      Phoinix::Utils::ScopedTimer creation{"RaytracerLayer constructor"};
      PRINT("Creating Application layer");
      m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
      for (size_t i = 0; i < m_ViewportWidth * m_ViewportHeight; i++)
      {
         auto color = glm::vec4(1.0f, .0f, .0f, 1.0f);
         m_ImageData[i] = Phoinix::Image::VecToRgba(color);
      }
      m_FinalImage = Phoinix::Image::Create(m_ViewportWidth, m_ViewportHeight, Phoinix::Format::RGBA, m_ImageData);
   }

   ~RaytracerLayer()
   {
      delete m_FinalImage;
      delete[] m_ImageData;
   }

   void OnUpdate() override
   {
      // Check if resize is needed
      if (m_FinalImage->GetWidth() == m_ViewportWidth && m_FinalImage->GetHeight() ==
      m_ViewportHeight) return;

      m_FinalImage->Resize(m_ViewportWidth, m_ViewportHeight);

      delete[] m_ImageData;
      m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];

      for (size_t i = 0; i < m_ViewportWidth * m_ViewportHeight; i++)
      {
          auto color = glm::vec4(1.0f, .0f, .0f, 1.0f);
          m_ImageData[i] = Phoinix::Image::VecToRgba(color);
      }

      // "Render"
      m_FinalImage->SetData(m_ImageData);
   }

   void OnEvent(Phoinix::Event& e) override
   {
      // PRINT("Application handling {}", e);
   }

   void OnImGUIUpdate() override
   {
      ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("Viewport");

      m_ViewportWidth = ImGui::GetContentRegionAvail().x;
      m_ViewportHeight = ImGui::GetContentRegionAvail().y;

      m_FinalImage->ImGuiBind();

      ImGui::End();
      ImGui::PopStyleVar();
   }

 private:
   Phoinix::Image* m_FinalImage;
   uint32_t* m_ImageData;
   uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
};

class Raytracer : public Phoinix::Application
{
 public:
   Raytracer() : Application()
   {
      AddLayer(new RaytracerLayer());
      ShouldShowImGuiDemo(false);
   }

   virtual ~Raytracer() = default;
};

Phoinix::Application* Phoinix::CreateApp()
{
   auto app = new Raytracer();
   // app->WithRendering(false);
   return app;
}
