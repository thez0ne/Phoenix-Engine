#pragma once
#include "pch.h"

#include "Platform/GLFW/GLFWWindow.h"
// #include <vulkan/vulkan.h>

namespace Phoinix
{
  class VulkanInstance
  {
  public:
    VulkanInstance();
    ~VulkanInstance();

    VulkanInstance(const VulkanInstance&) = delete;
    void operator=(const VulkanInstance&) = delete;
    VulkanInstance(VulkanInstance&&) = delete;
    VulkanInstance& operator=(VulkanInstance&&) = delete;

    [[nodiscard]] const VkInstance& GetInstance() const { return m_Instance; }

  private:
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;

    void CreateInstance();
    void SetupDebugMessenger();

    VkResult CreateDebugUtilsMessengerExt(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerExt(VkInstance instance,
                                       VkDebugUtilsMessengerEXT pDebugMessenger,
                                       const VkAllocationCallbacks* pAllocator);
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  };
}
