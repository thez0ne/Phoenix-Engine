#include <Phoenix.h>

#include "RayRenderer.h"
#include "Scene.h"
#include "Objects/Sphere.h"
#include "Utilities/Random.h"

class RaytracerLayer : public Phoenix::Layer
{
public:
  RaytracerLayer() : Layer("Sandbox Layer")
  {
    Phoenix::Utils::ScopedTimer creation{"RaytracerLayer constructor"};
    PRINT("Creating Application layer");

    // setting up materials
    {
      auto* mat = new Raytracing::Material();
      mat->name = "Red Smooth";
      mat->Albedo = glm::vec3(1.f, 0.f, 0.f);
      mat->Roughness = 0.f;
      m_Scene.AddMaterial(mat);
    }
    {
      auto* mat = new Raytracing::Material();
      mat->name = "Blue Slightly Rough";
      mat->Albedo = glm::vec3(0.f, 0.f, 1.f);
      mat->Roughness = .2f;
      m_Scene.AddMaterial(mat);
    }

    // setting up spheres
    {
      auto* sphere = new Raytracing::Sphere(glm::vec3(.0f, .0f, -1.f), 1.f);
      sphere->SetMaterialIndex(0);
      m_Scene.AddToScene(sphere);
    }

    // sphere as floor
    {
      auto* sphere = new Raytracing::Sphere(glm::vec3(0.f, -101.f, 0.f), 100.f);
      sphere->SetMaterialIndex(1);
      m_Scene.AddToScene(sphere);
    }

    m_Camera = Raytracing::Camera(glm::vec3(0.f, 0.f, 4.f), glm::vec4(.3f, .2f, .5f, 1.0f));

    // debug sphere for light
    // m_Scene.AddToScene(
    //   new Raytracing::Sphere(glm::vec3(-1.f, -1.f, -2.f), .2f, glm::vec4(1.f, 1.f, 1.f, 1.f)));
  }

  ~RaytracerLayer() = default;

  void OnUpdate() override
  {
    m_RenderTimer.Reset();

    m_Renderer.Resize();
    m_Renderer.Render(m_Scene, m_Camera);

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
        if (ImGui::BeginMenu("Add"))
        {
          if (ImGui::MenuItem("Sphere"))
          {
            m_Scene.AddToScene(new Raytracing::Sphere(glm::vec3(.0f, .0f, 0.f), 1.f));
            ImGui::EndMenu();
          }
          // TODO is there a way to dynamically find a list of available shapes??
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

    m_Camera.CameraSettings();

    ImGui::Separator();

    ImGui::Text("Frame Time: %.2f ms", m_FrameTime);

    ImGui::Separator();

    m_Scene.RenderHierarchy();

    ImGui::Separator();

    ImGui::InputText("filepath", &m_FileName);

    ImGui::End();
  }

private:
  Raytracing::RayRenderer m_Renderer;
  Raytracing::Scene m_Scene;
  Raytracing::Camera m_Camera;

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
  Raytracing::Random::Init();
  auto app = new Raytracer();
  app->WithRendering(false);
  return app;
}
