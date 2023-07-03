#include <Phoenix.h>

#include "RayRenderer.h"
#include "Scene.h"
#include "Objects/Sphere.h"

class RaytracerLayer : public Phoenix::Layer
{
public:
  RaytracerLayer() : Layer("Sandbox Layer")
  {
    Phoenix::Utils::ScopedTimer creation{"RaytracerLayer constructor"};
    PRINT("Creating Application layer");

    m_Scene.AddToScene(
      new Raytracing::Sphere(glm::vec3(.0f, .0f, -1.f), 0.5f, glm::vec4(1.f, 0.f, 0.f, 1.f)));
    m_Scene.AddToScene(
      new Raytracing::Sphere(glm::vec3(1.0f, .0f, -2.f), 1.f, glm::vec4(0.f, 1.f, 0.f, 1.f)));

    // debug sphere for light
    // m_Scene.AddToScene(
    //   new Raytracing::Sphere(glm::vec3(-1.f, -1.f, -2.f), .2f, glm::vec4(1.f, 1.f, 1.f, 1.f)));
  }

  ~RaytracerLayer() = default;

  void OnUpdate() override
  {
    m_RenderTimer.Reset();

    m_Renderer.Resize();
    m_Renderer.Render(m_Scene);

    m_FrameTime = m_RenderTimer.ElapsedMilliSeconds();
  }

  void OnEvent(Phoenix::Event& e) override
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
          m_Renderer.SaveImage(finalName);
          PRINT("Saving complete");
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Scene"))
      {
        if (ImGui::MenuItem("Add"))
        {
          // TODO make this a dropdown with supported shapes
          // TODO add shape to scene
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

    m_Renderer.OnImGuiUpdate();

    ImGui::End();
    ImGui::PopStyleVar();

    // Settings & Stats
    ImGui::Begin("Settings");

    ImGui::Text("Frame Time: %.2f ms", m_FrameTime);

    ImGui::Separator();

    // TODO cant change this, eventually move into the camera
    m_Renderer.OnBackgroundColourUpdate();

    ImGui::Separator();

    m_Scene.RenderHierarchy();

    ImGui::Separator();

    ImGui::InputText("filepath", &m_FileName);

    ImGui::End();
  }

private:
  Raytracing::RayRenderer m_Renderer;
  Raytracing::Scene m_Scene;

  std::string m_FileName = "output";
  Phoenix::Utils::Timer m_RenderTimer{};
  float m_FrameTime;
};

class Raytracer : public Phoenix::Application
{
public:
  Raytracer() : Application()
  {
    AddLayer(new RaytracerLayer());
    ShouldShowImGuiDemo(false);
  }

  virtual ~Raytracer() = default;
};

Phoenix::Application* Phoenix::CreateApp()
{
  auto app = new Raytracer();
  app->WithRendering(false);
  return app;
}
