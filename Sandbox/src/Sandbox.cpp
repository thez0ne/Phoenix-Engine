#include <Phoenix.h>

#include <glm/glm.hpp>

class SandboxLayer : public Phoenix::Layer
{
public:
  SandboxLayer() : Layer("Sandbox Layer")
  {
    Phoenix::Utils::ScopedTimer creation{"SandboxLayer constructor"};
    PRINT("Creating Application layer");
  }

  ~SandboxLayer() {}

  void OnUpdate() override {}

  void OnEvent(Phoenix::Event& e) override
  {
    // PRINT("Application handling {}", e);
  }

  void OnImGUIUpdate() override
  {
    ImGui::Begin("Test");
    ImGui::Text("This is a test");
    ImGui::End();
  }
};

class Sandbox : public Phoenix::Application
{
public:
  Sandbox() : Application() { AddLayer(new SandboxLayer()); }

  virtual ~Sandbox() = default;
};

Phoenix::Application* Phoenix::CreateApp()
{
  auto app = new Sandbox();
  return app;
}
