#include <Phoinix.h>

#include "RayRenderer.h"

class RaytracerLayer : public Phoinix::Layer
{
public:
  RaytracerLayer() : Layer("Sandbox Layer")
  {
    Phoinix::Utils::ScopedTimer creation{"RaytracerLayer constructor"};
    PRINT("Creating Application layer");
  }

  ~RaytracerLayer() {}

  void OnUpdate() override
  {
    m_RenderTimer.Reset();

    renderer.Resize();
    renderer.Render();

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
          auto finalName = m_FileName + ".png";
          PRINT("Saving image with name {}", finalName);
          renderer.SaveImage(finalName);
          PRINT("Saving complete");
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

    renderer.OnImGuiUpdate();

    ImGui::End();
    ImGui::PopStyleVar();

    // Settings & Stats
    ImGui::Begin("Settings");

    ImGui::Text("Frame Time: %.2f ms", m_FrameTime);

    ImGui::Separator();

    renderer.OnBackgroundColourUpdate();

    ImGui::Separator();

    ImGui::InputText("filepath", &m_FileName);

    ImGui::End();
  }

private:
  Raytracing::RayRenderer renderer;

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
