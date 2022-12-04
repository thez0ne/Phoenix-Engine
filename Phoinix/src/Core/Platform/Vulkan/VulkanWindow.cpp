#include "pch.h"
#include "VulkanWindow.h"

#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/KeyboardEvent.h"
#include "Core/Events/MouseEvent.h"

namespace Phoinix
{
    static bool GLFWInitialized = false;

    Window* Window::Create()
    {
        return new VulkanWindow();
    }

    VulkanWindow::VulkanWindow(const unsigned int w, const unsigned int h, const std::string& name)// : m_Data(w, h, name)
      {
         m_Data.title = name;
         m_Data.width = w;
         m_Data.height = h;
         ENGINE_TRACE("Creating GLFW Window with with Title: {}, Width: {}, Height: {}",
                        m_Data.title,
                        m_Data.width,
                        m_Data.height);
         Initialize();
         SetVSync(true);
         SetupCallbacks();
      }

    VulkanWindow::~VulkanWindow()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void VulkanWindow::Update()
    {
      glfwPollEvents();
      // glfwSwapBuffers(window); // TODO prob dont need this with our own swap chain code
    }

    void VulkanWindow::Initialize()
   {
      if (!GLFWInitialized)
      {
         int succ = glfwInit();
         PHOINIX_ASSERT(succ, "FAILED TO INIT GLFW!");
         GLFWInitialized = true;
      }
      // glfwInit();
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

      m_Window = glfwCreateWindow(m_Data.width, m_Data.height, m_Data.title.c_str(), nullptr, nullptr);

      //glfwSetWindowUserPointer(m_Window, this);
      glfwSetWindowUserPointer(m_Window, &m_Data);
      glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);
      glfwMakeContextCurrent(m_Window); // TODO need to check if imgui with vulkan needs this
   }

   void VulkanWindow::SetupCallbacks()
   {
      glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
         WindowCloseEvent e;
         data.callback(e);
      });

      /*
      glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focusStatus) {
         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

         switch (focusStatus)
         {
            case GLFW_TRUE:
            {
               WindowFocusEvent e;
               data.callback(e);
               break;
            }
            case GLFW_FALSE:
            {
               WindowUnfocusEvent e;
               data.callback(e);
               break;
            }
         }
      });

      glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

         WindowResizeEvent e(width, height);
         data.callback(e);
      });

      glfwSetKeyCallback(m_Window,
                        [](GLFWwindow* window, int keycode, int scancode, int action, int mods) {
                           WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                           switch (action)
                           {
                              case GLFW_PRESS:
                              {
                                 KeyDownEvent e(keycode, false);
                                 data.callback(e);
                                 break;
                              }
                              case GLFW_RELEASE:
                              {
                                 KeyUpEvent e(keycode);
                                 data.callback(e);
                                 break;
                              }
                              case GLFW_REPEAT:
                              {
                                 KeyDownEvent e(keycode, true);
                                 data.callback(e);
                                 break;
                              }
                           }
                        });

      glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int buttonID, int action, int mods) {
         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

         switch (action)
         {
            case GLFW_PRESS:
            {
               MouseButtonDownEvent e(buttonID);
               data.callback(e);
               break;
            }
            case GLFW_RELEASE:
            {
               MouseButtonUpEvent e(buttonID);
               data.callback(e);
               break;
            }
         }
      });

      glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

         MouseMovedEvent e((float)xPos, (float)yPos);
         data.callback(e);
      });

      glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xDelta, double yDelta) {
         WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

         MouseScrollEvent e((float)xDelta, (float)yDelta);
         data.callback(e);
      });*/
   }

   void VulkanWindow::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
   {
      // TODO move this into the SetupCallbacks function
       // TODO FIX THIS TO WORK WITH THE OTHER CALLBACKS
      auto app = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));
      app->m_FramebuffersResized = true;
   }

}
