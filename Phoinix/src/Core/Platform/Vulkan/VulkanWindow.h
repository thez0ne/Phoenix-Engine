#pragma once

#include "Core/Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Phoinix
{
    class VulkanWindow : public Window
    {
    public:

        VulkanWindow(const unsigned int w = 1280,
                     const unsigned int h = 720,
                     const std::string& title = "Vulkan Window");
        ~VulkanWindow();

        VulkanWindow(const VulkanWindow&) = delete;
        VulkanWindow& operator=(const VulkanWindow&) = delete;
        VulkanWindow(VulkanWindow&&) = delete;
        VulkanWindow& operator=(VulkanWindow&&) = delete;

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

    private:
      GLFWwindow* m_Window;

      struct WindowData
      {
         std::string title;
         unsigned int width, height;
         bool vSync;
         EventCallback callback;
      };

      WindowData m_Data;

      void Initialize();
      void SetupCallbacks();
      static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    };
}