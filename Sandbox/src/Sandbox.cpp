#include <Phoinix.h>

#include <glm/glm.hpp>

class SandboxLayer : public Phoinix::Layer
{
public:
  SandboxLayer() : Layer("Sandbox Layer")
  {
    Phoinix::Utils::ScopedTimer creation{"SandboxLayer constructor"};
    PRINT("Creating Application layer");
  }

  ~SandboxLayer() {}

  void OnUpdate() override {}

  void OnEvent(Phoinix::Event& e) override
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

class Sandbox : public Phoinix::Application
{
public:
  Sandbox() : Application() { AddLayer(new SandboxLayer()); }

  virtual ~Sandbox() = default;
};

Phoinix::Application* Phoinix::CreateApp()
{
  auto app = new Sandbox();
  return app;
}
