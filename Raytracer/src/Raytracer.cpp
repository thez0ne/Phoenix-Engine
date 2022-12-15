#include <Phoinix.h>

class RaytracerLayer : public Phoinix::Layer
{
 public:
   RaytracerLayer() : Layer("Sandbox Layer")
   {
      PRINT("Creating Application layer");
   }

   void OnUpdate() override
   {
      // PRINT("We in the app boys");
   }

   void OnEvent(Phoinix::Event& e) override
   {
      // PRINT("Application handling {}", e);
   }

   void OnImGUIUpdate() override
   {
      // ImGui::Begin("Hello, world!");
      // ImGui::Text("This is some useful text.");
      // ImGui::End();
   }
};

class Raytracer : public Phoinix::Application
{
 public:
   Raytracer() : Application()
   {
      AddLayer(new RaytracerLayer());
      // ImGui::SetCurrentContext(ImgGuiCtx);
   }

   virtual ~Raytracer() = default;
};

Phoinix::Application* Phoinix::CreateApp()
{
   return new Raytracer();
}
