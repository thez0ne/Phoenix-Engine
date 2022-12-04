#include "pch.h"
#include "Application.h"
// #include "Core/Log.h"
//#include "Core/ImGui/ImGuiLayer.h"

// #include <GLFW/glfw3.h>

namespace Phoinix
{
    Application* Application::instance = nullptr;

    Application::Application()// : layerStack()
    {
        PHOINIX_ASSERT(
            !instance,
            "Application should be a singleton!"); // TODO can i move this into a base singleton class?

        instance = this;
        // window = Window::create();
        // window->setEventCallback(std::bind(&Application::onEvent, this, std::placeholders::_1));
        // renderer = Renderer::create();

        // _ImGuiLayer = new ImGuiLayer();
        // addOverlay(_ImGuiLayer);

        // ImgGuiCtx = _ImGuiLayer->getImGuiCtx();
    }

    Application::~Application()
    {
        // delete window;
        // delete renderer;
    }

    void Application::Run()
    {
        ENGINE_INFO("Application is starting up");

        // while (isRunning)
        // {
        //       renderer->startFrame();

        //       for (Layer* layer : layerStack)
        //          layer->onUpdate();

        //       _ImGuiLayer->ImGuiInitFrame();
        //       for (Layer* layer : layerStack)
        //          layer->onImGUIUpdate();
        //       _ImGuiLayer->ImGuiRenderFrame();

        //       renderer->render();

        //       renderer->endFrame();

        //       window->update();
        // }
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<WindowCloseEvent>(
            std::bind(&Application::OnClose, this, std::placeholders::_1));

        for (LayerIterator it = layerStack.End(); it != layerStack.Begin();)
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

    bool Application::OnClose(WindowCloseEvent& e)
    {
        isRunning = false;
        return true;
    }

}

