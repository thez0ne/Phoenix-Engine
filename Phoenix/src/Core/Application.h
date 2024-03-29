#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyboardEvent.h"
#include "Events/MouseEvent.h"
#include "Window.h"
#include "Graphics/API/Renderer.h"
#include "LayerStack.h"
#include "ImGui/ImGuiLayer.h"

namespace Phoenix
{
  class Application
  {
  public:
    Application();
    virtual ~Application();

    static void Init();
    void Run();

    void OnEvent(Event& e);

    void AddLayer(Layer* layer);
    void AddOverlay(Layer* layer);
    void RemoveLayer(Layer* layer);

    inline Window& GetWindow() { return *window; }
    Renderer& GetRenderer() { return *renderer; }
    static Application* GetInstance() { return instance; }

    void WithRendering(bool status) { shouldRender = status; }

  private:
    static Application* _instance;
    LayerStack layerStack;

    ImGuiLayer* _ImGuiLayer;

    Window* window;
    Renderer* renderer;
    bool isRunning = true;

    static Application* instance;

    bool shouldRender = true;

  private:
    bool OnClose(WindowCloseEvent& e);

  protected:
    void ShouldShowImGuiDemo(bool status) { _ImGuiLayer->ShouldShowDemoWindow(status); }
  };

  Application* CreateApp();
}
