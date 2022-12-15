#pragma once
#include "pch.h"
#include "Platform/Vulkan/VulkanDevice.h"
#include <vulkan/vulkan.h>

// TODO temp
#include <glm/glm.hpp>

namespace Phoinix
{
   // TODO move to file?
   struct UniformBufferObject
   {
      glm::mat4 model;
      glm::mat4 view;
      glm::mat4 projection;
   };

   uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

   void CreateBuffer(VkDeviceSize size,
                     VkBufferUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VkBuffer& buffer,
                     VkDeviceMemory& bufferMemory);

   void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
}
