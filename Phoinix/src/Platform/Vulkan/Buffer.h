#pragma once
#include "pch.h"
#include "VulkanDevice.h"
#include <vulkan/vulkan.h>

namespace Phoinix
{
   uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice,
                           uint32_t typeFilter,
                           VkMemoryPropertyFlags properties);

   void CreateBuffer(VulkanDevice& device,
                     VkDeviceSize size,
                     VkBufferUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VkBuffer& buffer,
                     VkDeviceMemory& bufferMemory);

   void CopyBuffer(VulkanDevice& device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
}
