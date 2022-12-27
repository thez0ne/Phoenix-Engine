#pragma once

#include "Core/Window.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Phoinix
{
  class GLFWWindow : public Window
  {
  public:
    GLFWWindow(const unsigned int w = 1280,
               const unsigned int h = 720,
               const std::string& title = "Vulkan Window");
    ~GLFWWindow();

    GLFWWindow(const GLFWWindow&) = delete;
    GLFWWindow& operator=(const GLFWWindow&) = delete;
    GLFWWindow(GLFWWindow&&) = delete;
    GLFWWindow& operator=(GLFWWindow&&) = delete;

    void Update() override;

    inline unsigned int GetWidth() const override { return m_Data.width; }
    inline unsigned int GetHeight() const override { return m_Data.height; }

    inline void SetEventCallback(const EventCallback& _callback) override
    {
      m_Data.callback = _callback;
    }

    void SetVSync(bool enabled) override
    {
      if (enabled)
        glfwSwapInterval(1);
      else
        glfwSwapInterval(0);

      m_Data.vSync = enabled;
    };
    bool GetVSync() const override { return m_Data.vSync; };

    inline void* GetWindow() const override { return m_Window; }

    bool m_FramebuffersResized = false;

    struct WindowData
    {
      std::string title;
      unsigned int width, height;
      bool vSync;
      EventCallback callback;
    };

  private:
    GLFWwindow* m_Window;
    WindowData m_Data;

    void Initialize();
    void SetupCallbacks();
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
  };
}
