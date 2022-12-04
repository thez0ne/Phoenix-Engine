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

   VulkanWindow::VulkanWindow(const unsigned int w,
                              const unsigned int h,
                              const std::string& name) // : m_Data(w, h, name)
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

      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

      m_Window =
         glfwCreateWindow(m_Data.width, m_Data.height, m_Data.title.c_str(), nullptr, nullptr);

      glfwSetWindowUserPointer(m_Window, this);
      glfwMakeContextCurrent(m_Window); // TODO need to check if imgui with vulkan needs this
   }

   void VulkanWindow::SetupCallbacks()
   {
      glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
         auto vkWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));

         WindowCloseEvent e;
         vkWindow->m_Data.callback(e);
      });

      glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focusStatus) {
         auto vkWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));

         switch (focusStatus)
         {
            case GLFW_TRUE:
            {
               WindowFocusEvent e;
               vkWindow->m_Data.callback(e);
               break;
            }
            case GLFW_FALSE:
            {
               WindowUnfocusEvent e;
               vkWindow->m_Data.callback(e);
               break;
            }
         }
      });

      glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
         auto vkWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));

         WindowResizeEvent e(width, height);
         vkWindow->m_Data.callback(e);
      });

      glfwSetKeyCallback(
         m_Window, [](GLFWwindow* window, int keycode, int scancode, int action, int mods) {
            auto vkWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
               case GLFW_PRESS:
               {
                  KeyDownEvent e(keycode, false);
                  vkWindow->m_Data.callback(e);
                  break;
               }
               case GLFW_RELEASE:
               {
                  KeyUpEvent e(keycode);
                  vkWindow->m_Data.callback(e);
                  break;
               }
               case GLFW_REPEAT:
               {
                  KeyDownEvent e(keycode, true);
                  vkWindow->m_Data.callback(e);
                  break;
               }
            }
         });

      glfwSetMouseButtonCallback(
         m_Window, [](GLFWwindow* window, int buttonID, int action, int mods) {
            auto vkWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
               case GLFW_PRESS:
               {
                  MouseButtonDownEvent e(buttonID);
                  vkWindow->m_Data.callback(e);
                  break;
               }
               case GLFW_RELEASE:
               {
                  MouseButtonUpEvent e(buttonID);
                  vkWindow->m_Data.callback(e);
                  break;
               }
            }
         });

      glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
         auto vkWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));

         MouseMovedEvent e((float)xPos, (float)yPos);
         vkWindow->m_Data.callback(e);
      });

      glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xDelta, double yDelta) {
         auto vkWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));

         MouseScrollEvent e((float)xDelta, (float)yDelta);
         vkWindow->m_Data.callback(e);
      });

      glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
         auto vkWindow = reinterpret_cast<VulkanWindow*>(glfwGetWindowUserPointer(window));
         vkWindow->m_FramebuffersResized = true;
      });
   }
}
