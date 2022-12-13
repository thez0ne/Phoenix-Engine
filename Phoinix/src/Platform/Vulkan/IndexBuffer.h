#pragma once
#include "pch.h"

#include "VulkanDevice.h"
#include "Buffer.h"

namespace Phoinix
{
   class IndexBuffer
   {
   public:
      IndexBuffer(VulkanDevice& device, std::vector<uint16_t> indices);
      ~IndexBuffer();

      void Bind(VkCommandBuffer commandBuffer);
   private:
      VkBuffer m_IndexBuffer;
      VkDeviceMemory m_IndexBufferMemory;
      VulkanDevice& m_Device;
      void* m_Data;
   };
}