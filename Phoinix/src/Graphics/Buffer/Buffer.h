#pragma once
#include "pch.h"
#include "Platform/Vulkan/VulkanDevice.h"
#include <vulkan/vulkan.h>

// TODO temp
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

namespace Phoinix
{
   // TODO move to file?
   struct UniformBufferObject
   {
      alignas(16) glm::mat4 model;
      alignas(16) glm::mat4 view;
      alignas(16) glm::mat4 projection;
   };

   uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

   void CreateBuffer(VkDeviceSize size,
                     VkBufferUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VkBuffer& buffer,
                     VkDeviceMemory& bufferMemory);

   void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
}
