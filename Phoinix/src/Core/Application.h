#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Window.h"
#include "Renderer.h"
#include "LayerStack.h"
#include "ImGui/ImGuiLayer.h"

namespace Phoinix
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

      inline Window& GetWindow() { return *window; }
      Renderer& GetRenderer() { return *renderer; }
      static Application* GetInstance() { return instance; }

    protected:
    //   ImGuiContext* ImgGuiCtx;

    private:
      static Application* _instance;
      LayerStack layerStack;

      ImGuiLayer* _ImGuiLayer;

      Window* window;
      Renderer* renderer;
      bool isRunning = true;

      static Application* instance;

    private:
      bool OnClose(WindowCloseEvent& e);
   };

   Application* CreateApp();
}
