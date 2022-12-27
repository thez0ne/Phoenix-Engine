#include <Phoinix.h>

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
    m_FinalImage =
      Phoinix::Image::Create(m_ViewportWidth, m_ViewportHeight, Phoinix::Format::RGBA, m_ImageData);
  }

  ~RaytracerLayer()
  {
    delete m_FinalImage;
    delete[] m_ImageData;
  }

  void OnUpdate() override
  {
    m_RenderTimer.Reset();

    // Check if resize is needed
    if (m_FinalImage->GetWidth() == m_ViewportWidth &&
        m_FinalImage->GetHeight() == m_ViewportHeight)
      return;

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

    m_FrameTime = m_RenderTimer.ElapsedMilliSeconds();
  }

  void OnEvent(Phoinix::Event& e) override
  {
    // PRINT("Application handling {}", e);
    // TODO get file save assigned to keyboard shortcut
  }

  void OnImGUIUpdate() override
  {
    // Main Menu Bar
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("Save"))
        {
          PRINT("Saving image");
          m_FinalImage->Save(m_FileName);
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }

    // Dockspace
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    // Raytracer Viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse);

    m_ViewportWidth = ImGui::GetContentRegionAvail().x;
    m_ViewportHeight = ImGui::GetContentRegionAvail().y;

    m_FinalImage->ImGuiBind();

    ImGui::End();
    ImGui::PopStyleVar();

    // Settings & Stats
    ImGui::Begin("Settings");

    ImGui::Text("Frame Time: %.2f ms", m_FrameTime);

    ImGui::Separator();

    ImGui::InputText("filepath", &m_FileName);

    ImGui::End();
  }

private:
  Phoinix::Image* m_FinalImage;
  uint32_t* m_ImageData;
  uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

  std::string m_FileName = "output";
  Phoinix::Utils::Timer m_RenderTimer{};
  float m_FrameTime;
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
  app->WithRendering(false);
  return app;
}
