#include "VulkanDevice.h"

#include "VulkanValidation.h"

namespace Phoinix
{
   VulkanDevice::VulkanDevice(const VkInstance& instance, GLFWwindow* window) :
      m_Instance(instance), m_Window(window)
   {
      CreateSurface();

      uint32_t deviceCount = 0;
      vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

      if (deviceCount == 0)
      {
         ENGINE_ERR("Failed to find Vulkan compatible device");
         std::exit(-3);
      }

      std::vector<VkPhysicalDevice> devices(deviceCount);
      vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

      for (const auto& _device : devices)
      {
         if (IsDeviceSuitable(_device))
         {
            m_PhysicalDevice = _device;
            break;
         }
      }

      if (m_PhysicalDevice == VK_NULL_HANDLE)
      {
         ENGINE_ERR("Failed to find suitable GPU");
         std::exit(-3);
      }

      CreateLogicalDevice();
      CreateSwapChain();
      CreateImageViews();
      CreateRenderPass();
      CreateCommandPool();
   }

   VulkanDevice::~VulkanDevice()
   {
      vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
      vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
      for (const auto view : m_SwapChainImageViews)
      {
         vkDestroyImageView(m_Device, view, nullptr);
      }
      vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
      vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
      vkDestroyDevice(m_Device, nullptr);
   }

   bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device)
   {
      QueueFamilyIndices indices = FindQueueFamilies(device);

      VkPhysicalDeviceProperties deviceProperties{};
      vkGetPhysicalDeviceProperties(device, &deviceProperties);

      VkPhysicalDeviceFeatures deviceFeatures{};
      vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

      bool extensionsSupported = CheckDeviceExtensionsSupport(device);

      bool swapChainSupported = false;
      if (extensionsSupported)
      {
         SwapChainSupportDetails swapChainSupport = FindSwapChainSupport(device);
         swapChainSupported =
            !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
      }

      return indices.HasValue() &&
         deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
         deviceFeatures.geometryShader && extensionsSupported && swapChainSupported;
   }

   QueueFamilyIndices VulkanDevice::FindQueueFamilies(const VkPhysicalDevice device) const
   {
      QueueFamilyIndices indices;

      uint32_t queueFamilyCount = 0;
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

      std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
      vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

      int i = 0;
      for (const auto& queueFamily : queueFamilies)
      {
         if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
         {
            indices.graphicsFamily = i;
         }

         VkBool32 presentSupport = false;
         vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

         if (presentSupport)
         {
            indices.presentFamily = i;
         }

         if (indices.HasValue())
         {
            break;
         }

         i++;
      }

      return indices;
   }

   void VulkanDevice::CreateLogicalDevice()
   {
      QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

      std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
      std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                                indices.presentFamily.value()};

      float queuePriority = 1.0f;
      for (uint32_t queueFamily : uniqueQueueFamilies)
      {
         VkDeviceQueueCreateInfo queueCreateInfo{};
         queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
         queueCreateInfo.queueFamilyIndex = queueFamily;
         queueCreateInfo.queueCount = 1;
         queueCreateInfo.pQueuePriorities = &queuePriority;

         queueCreateInfos.push_back(queueCreateInfo);
      }

      VkPhysicalDeviceFeatures deviceFeatures{};

      VkDeviceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
      createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
      createInfo.pQueueCreateInfos = queueCreateInfos.data();
      createInfo.queueCreateInfoCount = 1;
      createInfo.pEnabledFeatures = &deviceFeatures;

      createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
      createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

      if (enable_validation_layers)
      {
         createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
         createInfo.ppEnabledLayerNames = validation_layers.data();
      }
      else
      {
         createInfo.enabledLayerCount = 0;
      }

      if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create logical device");
         std::exit(-3);
      }

      vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
      vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
   }

   void VulkanDevice::CreateSurface()
   {
      if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create window surface");
         std::exit(-3);
      }
   }

   bool VulkanDevice::CheckDeviceExtensionsSupport(VkPhysicalDevice device) const
   {
      uint32_t extensionCount;
      vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

      std::vector<VkExtensionProperties> availableExtensions(extensionCount);
      vkEnumerateDeviceExtensionProperties(
         device, nullptr, &extensionCount, availableExtensions.data());

      std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(),
                                               m_DeviceExtensions.end());

      for (const auto& extension : availableExtensions)
      {
         requiredExtensions.erase(extension.extensionName);
      }

      return requiredExtensions.empty();
   }

   SwapChainSupportDetails VulkanDevice::FindSwapChainSupport(VkPhysicalDevice device) const
   {
      SwapChainSupportDetails details;

      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

      uint32_t formatCount;
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

      if (formatCount != 0)
      {
         details.formats.resize(formatCount);
         vkGetPhysicalDeviceSurfaceFormatsKHR(
            device, m_Surface, &formatCount, details.formats.data());
      }

      uint32_t presentModeCount;
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

      if (presentModeCount != 0)
      {
         details.presentModes.resize(presentModeCount);
         vkGetPhysicalDeviceSurfacePresentModesKHR(
            device, m_Surface, &presentModeCount, details.presentModes.data());
      }

      return details;
   }

   VkSurfaceFormatKHR
   VulkanDevice::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
   {
      for (const auto& availableFormat : availableFormats)
      {
         if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
             availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
         {
            return availableFormat;
         }
      }

      // TODO: if the previous format is not avaialble, should filter out the next best one instead
      // of this
      return availableFormats[0];
   }

   VkPresentModeKHR
   VulkanDevice::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
   {
      for (const auto& availablePresentMode : availablePresentModes)
      {
         if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
         {
            return availablePresentMode;
         }
      }

      // TODO: pick optimal present mode depending on device type (i.e. FIFO for mobile devices for
      // less power consumption)
      return VK_PRESENT_MODE_FIFO_KHR;
   }

   VkExtent2D VulkanDevice::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
   {
      if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
      {
         return capabilities.currentExtent;
      }
      else
      {
         int width, height;
         glfwGetFramebufferSize(m_Window, &width, &height);

         VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

         actualExtent.width = std::clamp(actualExtent.width,
                                         capabilities.minImageExtent.width,
                                         capabilities.maxImageExtent.width);
         actualExtent.height = std::clamp(actualExtent.height,
                                          capabilities.minImageExtent.height,
                                          capabilities.maxImageExtent.height);
         return actualExtent;
      }
   }

   void VulkanDevice::CreateSwapChain()
   {
      const SwapChainSupportDetails swapChainSupport = FindSwapChainSupport(m_PhysicalDevice);

      const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
      const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
      const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

      uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
      if (swapChainSupport.capabilities.maxImageCount > 0 &&
          imageCount > swapChainSupport.capabilities.maxImageCount)
      {
         imageCount = swapChainSupport.capabilities.maxImageCount;
      }

      VkSwapchainCreateInfoKHR createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
      createInfo.surface = m_Surface;
      createInfo.minImageCount = imageCount;
      createInfo.imageFormat = surfaceFormat.format;
      createInfo.imageColorSpace = surfaceFormat.colorSpace;
      createInfo.imageExtent = extent;
      // WARNING: always 1 unless making a stereoscopic 3D app
      createInfo.imageArrayLayers = 1;
      // WARNING: should use VK_IMAGE_USAGE_TRANSFER_DST_BIT instead for post processing then
      // transfer result to swap chain image
      createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

      const QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);
      const uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                             indices.presentFamily.value()};

      if (indices.graphicsFamily != indices.presentFamily)
      {
         createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
         createInfo.queueFamilyIndexCount = 2;
         createInfo.pQueueFamilyIndices = queueFamilyIndices;
      }
      else
      {
         createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
         createInfo.queueFamilyIndexCount = 0;
         createInfo.pQueueFamilyIndices = nullptr;
      }

      createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
      createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // ignores alpha channel
      createInfo.presentMode = presentMode;
      createInfo.clipped = VK_TRUE;
      createInfo.oldSwapchain = VK_NULL_HANDLE;

      if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create swapchain");
         std::exit(-3);
      }

      vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
      m_SwapChainImages.resize(imageCount);
      vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

      m_SwapChainImageFormat = surfaceFormat.format;
      m_SwapChainExtent = extent;
   }

   void VulkanDevice::CreateImageViews()
   {
      m_SwapChainImageViews.resize(m_SwapChainImages.size());

      for (size_t i = 0; i < m_SwapChainImages.size(); i++)
      {
         VkImageViewCreateInfo createInfo{};
         createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
         createInfo.image = m_SwapChainImages[i];
         createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // treated as 2d texture
         createInfo.format = m_SwapChainImageFormat;

         // currently maps the colours to their defaults
         // this could be used to map everything to 1 colour to make monochrom textures
         createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
         createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
         createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
         createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

         createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
         createInfo.subresourceRange.baseMipLevel = 0;
         createInfo.subresourceRange.levelCount = 1;
         createInfo.subresourceRange.baseArrayLayer = 0;
         createInfo.subresourceRange.layerCount = 1; // might want more layers for stereoscopic 3d

         if (vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]) !=
             VK_SUCCESS)
         {
            ENGINE_ERR("Failed to create image view");
            std::exit(-3);
         }
      }
   }

   void VulkanDevice::CleanupSwapChain()
   {
      for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
      {
         vkDestroyImageView(m_Device, m_SwapChainImageViews[i], nullptr);
      }

      vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
   }

   void VulkanDevice::CreateRenderPass()
   {
      // TODO: missing the depth buffer attachment description

      // Render pass - Colour
      VkAttachmentDescription colorAttachment{};
      colorAttachment.format = m_SwapChainImageFormat;
      colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
      colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
      colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
      colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
      colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
      colorAttachment.initialLayout =
         VK_IMAGE_LAYOUT_UNDEFINED; // don't care what the previous format was
      colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

      VkAttachmentReference colorAttachmentRef{};
      colorAttachmentRef.attachment = 0;
      colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      VkSubpassDescription subpass{};
      subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
      subpass.colorAttachmentCount = 1;
      subpass.pColorAttachments = &colorAttachmentRef;

      VkSubpassDependency dependency{};
      dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
      dependency.dstSubpass = 0;
      dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      dependency.srcAccessMask = 0;
      dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

      VkRenderPassCreateInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
      renderPassInfo.attachmentCount = 1;
      renderPassInfo.pAttachments = &colorAttachment;
      renderPassInfo.subpassCount = 1;
      renderPassInfo.pSubpasses = &subpass;
      renderPassInfo.dependencyCount = 1;
      renderPassInfo.pDependencies = &dependency;

      if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create render pass");
         std::exit(-3);
      }
   }

   void VulkanDevice::CreateCommandPool()
   {
      QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_PhysicalDevice);

      VkCommandPoolCreateInfo commandPoolCreateInfo{};
      commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
      commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
      commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

      if (vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_CommandPool) !=
          VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create command pool");
         std::exit(-3);
      }
   }
}
