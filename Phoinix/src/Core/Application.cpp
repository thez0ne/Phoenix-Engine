#include "pch.h"
#include "Application.h"

#include "Platform/Vulkan/VulkanFunction.h"

namespace Phoinix
{
   Application* Application::instance = nullptr;

   Application::Application() : layerStack()
   {
      PHOINIX_ASSERT(
         !instance,
         "Application should be a singleton!"); // TODO can i move this into a base singleton class?

      instance = this;
      window = Window::Create();
      window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

      Graphics::Vulkan::MakeDefault(); // TODO can prob be moved elsewhere to allow for switching APIs
      renderer = Renderer::CreateFunc(window);

      _ImGuiLayer = new ImGuiLayer();
      AddOverlay(_ImGuiLayer);
   }

   Application::~Application()
   {
      RemoveLayer(_ImGuiLayer);
      delete window;
      delete renderer;
   }

   void Application::Run()
   {
      ENGINE_INFO("Application is starting up");

      while (isRunning)
      {
         window->Update();

         for (Layer* layer : layerStack)
            layer->OnUpdate();

         renderer->BeginRender();

         _ImGuiLayer->ImGuiInitFrame();
         for (Layer* layer : layerStack)
         {
            layer->OnImGUIUpdate();
         }
         renderer->Render();
         _ImGuiLayer->ImGuiRenderFrame();

         // renderer->DrawFrame();

         renderer->EndRender();
      }
      // TODO make sure command buffer is emptied and cleaned up before exiting
      //renderer->EndRender();
      ENGINE_INFO("Application is closing down");
   }

   void Application::OnEvent(Event& e)
   {
      EventDispatcher dispatcher(e);
      dispatcher.dispatch<WindowCloseEvent>(
         std::bind(&Application::OnClose, this, std::placeholders::_1));

      for (LayerIterator it = layerStack.end(); it != layerStack.begin();)
      {
         (*--it)->OnEvent(e);
         if (e.handled)
            break;
      }
   }

   void Application::AddLayer(Layer* layer)
   {
      layerStack.PushLayer(layer);
   }

   void Application::AddOverlay(Layer* layer)
   {
      layerStack.PushOverlay(layer);
   }

   void Application::RemoveLayer(Layer* layer)
   {
      layerStack.PopLayer(layer);
   }

   bool Application::OnClose(WindowCloseEvent& e)
   {
      isRunning = false;
      return true;
   }

}
