#include <Phoinix.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

class RaytracerLayer : public Phoinix::Layer
{
 public:
   RaytracerLayer() : Layer("Sandbox Layer")
   {
      Phoinix::Utils::ScopedTimer creation{"RaytracerLayer"};
      PRINT("Creating Application layer");
      m_ViewportWidth = 800;
      m_ViewportHeight = 600;
      m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
      for (size_t i = 0; i < m_ViewportWidth * m_ViewportHeight; i++)
      {
         auto color = glm::vec4(1.0f, .0f, .0f, 1.0f);
         m_ImageData[i] = Phoinix::Image::VecToRgba(color);
      }
      m_FinalImage = Phoinix::Image::Create(800, 600, Phoinix::Format::RGBA, m_ImageData);
   }

   ~RaytracerLayer()
   {
      delete m_FinalImage;
      delete[] m_ImageData;
   }

   void OnUpdate() override
   {
      // PRINT("We in the app boys");
      /*
      PRINT("My sizes are {}x{}", m_ViewportWidth, m_ViewportHeight);

      delete[] m_ImageData;
      m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];

      for (size_t i = 0; i < m_ViewportWidth * m_ViewportHeight; i++)
      {
          auto color = glm::vec4(1.0f, .0f, .0f, 1.0f);
          m_ImageData[i] = Phoinix::Image::VecToRgba(color);
      }

      m_FinalImage->Resize(m_ViewportWidth, m_ViewportHeight);
      */
   }

   void OnEvent(Phoinix::Event& e) override
   {
      // PRINT("Application handling {}", e);
   }

   void OnImGUIUpdate() override
   {
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
      ImGui::Begin("Viewport");

      m_ViewportWidth = ImGui::GetContentRegionAvail().x;
      m_ViewportHeight = ImGui::GetContentRegionAvail().y;

      m_FinalImage->ImGuiBind();

      ImGui::End();
      ImGui::PopStyleVar();
   }

 private:
   // std::shared_ptr<Phoinix::Image> m_FinalImage;
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
   }

   virtual ~Raytracer() = default;
};

Phoinix::Application* Phoinix::CreateApp()
{
   return new Raytracer();
}
