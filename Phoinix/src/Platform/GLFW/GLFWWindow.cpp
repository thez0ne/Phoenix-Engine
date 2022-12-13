#include "pch.h"
#include "GLFWWindow.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyboardEvent.h"
#include "Events/MouseEvent.h"

namespace Phoinix
{
   static bool GLFWInitialized = false;

   Window* Window::Create()
   {
      return new GLFWWindow();
   }

   GLFWWindow::GLFWWindow(const unsigned int w,
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
      // SetVSync(true);
      SetupCallbacks();
   }

   GLFWWindow::~GLFWWindow()
   {
      glfwDestroyWindow(m_Window);
      glfwTerminate();
   }

   void GLFWWindow::Update()
   {
      glfwPollEvents();
      // glfwSwapBuffers(window); // TODO prob dont need this with our own swap chain code
   }

   void GLFWWindow::Initialize()
   {
      if (!GLFWInitialized)
      {
         int succ = glfwInit();
         PHOINIX_ASSERT(succ, "FAILED TO INIT GLFW!");
         GLFWInitialized = true;
      }

      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

      m_Window =
         glfwCreateWindow(m_Data.width, m_Data.height, m_Data.title.c_str(), nullptr, nullptr);

      glfwSetWindowUserPointer(m_Window, this);
   }

   void GLFWWindow::SetupCallbacks()
   {
      glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
         auto glfwWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));

         WindowCloseEvent e;
         glfwWindow->m_Data.callback(e);
      });

      glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focusStatus) {
         auto glfwWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));

         switch (focusStatus)
         {
            case GLFW_TRUE:
            {
               WindowFocusEvent e;
               glfwWindow->m_Data.callback(e);
               break;
            }
            case GLFW_FALSE:
            {
               WindowUnfocusEvent e;
               glfwWindow->m_Data.callback(e);
               break;
            }
         }
      });

      glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
         auto glfwWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));

         WindowResizeEvent e(width, height);
         glfwWindow->m_Data.callback(e);
      });

      glfwSetKeyCallback(
         m_Window, [](GLFWwindow* window, int keycode, int scancode, int action, int mods) {
            auto glfwWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
               case GLFW_PRESS:
               {
                  KeyDownEvent e(keycode, false);
                  glfwWindow->m_Data.callback(e);
                  break;
               }
               case GLFW_RELEASE:
               {
                  KeyUpEvent e(keycode);
                  glfwWindow->m_Data.callback(e);
                  break;
               }
               case GLFW_REPEAT:
               {
                  KeyDownEvent e(keycode, true);
                  glfwWindow->m_Data.callback(e);
                  break;
               }
            }
         });

      glfwSetMouseButtonCallback(
         m_Window, [](GLFWwindow* window, int buttonID, int action, int mods) {
            auto glfwWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
               case GLFW_PRESS:
               {
                  MouseButtonDownEvent e(buttonID);
                  glfwWindow->m_Data.callback(e);
                  break;
               }
               case GLFW_RELEASE:
               {
                  MouseButtonUpEvent e(buttonID);
                  glfwWindow->m_Data.callback(e);
                  break;
               }
            }
         });

      glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
         auto glfwWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));

         MouseMovedEvent e((float)xPos, (float)yPos);
         glfwWindow->m_Data.callback(e);
      });

      glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xDelta, double yDelta) {
         auto glfwWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));

         MouseScrollEvent e((float)xDelta, (float)yDelta);
         glfwWindow->m_Data.callback(e);
      });

      glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
         auto glfwWindow = reinterpret_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
         glfwWindow->m_FramebuffersResized = true;
      });
   }
}
