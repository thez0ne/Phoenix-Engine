#pragma once
#include "pch.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Phoinix
{
   struct QueueFamilyIndices
   {
      std::optional<uint32_t> graphicsFamily;
      std::optional<uint32_t> presentFamily;

      [[nodiscard]] bool HasValue() const
      {
         return graphicsFamily.has_value() && presentFamily.has_value();
      }
   };

   struct SwapChainSupportDetails
   {
      VkSurfaceCapabilitiesKHR capabilities{};
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
   };

   // TODO: add choosing the optimal device instead of just picking the first discrete gpu
   // encountered check //
   // https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
   class VulkanDevice
   {
    public:
      VulkanDevice(const VkInstance& instance, GLFWwindow* window);
      ~VulkanDevice();
      VulkanDevice(const VulkanDevice&) = delete;
      VulkanDevice& operator=(const VulkanDevice&) = delete;
      VulkanDevice(VulkanDevice&&) = delete;
      VulkanDevice& operator=(VulkanDevice&&) = delete;

      [[nodiscard]] VkDevice GetDevice() const { return m_Device; }
      [[nodiscard]] VkRenderPass GetRenderPass() const { return m_RenderPass; }
      [[nodiscard]] VkExtent2D GetExtent() const { return m_SwapChainExtent; }
      [[nodiscard]] uint32_t GetExtentWidth() const { return m_SwapChainExtent.width; }
      [[nodiscard]] uint32_t GetExtentHeight() const { return m_SwapChainExtent.height; }
      [[nodiscard]] VkSwapchainKHR GetSwapChain() const { return m_SwapChain; }
      [[nodiscard]] std::vector<VkImageView> GetSwapChainImageViews() const
      {
         return m_SwapChainImageViews;
      }
      [[nodiscard]] VkCommandPool GetCommandPool() const { return m_CommandPool; }
      [[nodiscard]] VkQueue GetPresentQueue() const { return m_PresentQueue; }
      [[nodiscard]] VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }

      void CreateSwapChain();
      void CreateImageViews();

      void CleanupSwapChain();

    private:
      VkSwapchainKHR m_SwapChain{};
      VkSurfaceKHR m_Surface{};
      VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
      VkDevice m_Device{};
      VkQueue m_PresentQueue{};  // TODO: is here a good spot?
      VkQueue m_GraphicsQueue{}; // TODO: is here a good spot?

      std::vector<VkImage> m_SwapChainImages; // TODO is this a good spot
      VkFormat m_SwapChainImageFormat{};      // TODO is this a good spot
      VkExtent2D m_SwapChainExtent{};         // TODO is this a good spot

      std::vector<VkImageView> m_SwapChainImageViews;

      const std::vector<const char*> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

      VkRenderPass m_RenderPass{};

      const VkInstance& m_Instance;
      GLFWwindow* m_Window;

      VkCommandPool m_CommandPool;

      bool IsDeviceSuitable(VkPhysicalDevice device);
      QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) const;
      void CreateLogicalDevice();
      void CreateSurface();
      bool CheckDeviceExtensionsSupport(VkPhysicalDevice device) const;
      SwapChainSupportDetails FindSwapChainSupport(VkPhysicalDevice device) const;
      VkSurfaceFormatKHR
      ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
      VkPresentModeKHR
      ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
      VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
      void CreateRenderPass();
      void CreateCommandPool();
   };
}
