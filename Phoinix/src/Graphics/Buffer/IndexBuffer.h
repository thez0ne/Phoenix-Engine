#pragma once
#include "pch.h"

#include "Platform/Vulkan/VulkanDevice.h"
#include "Buffer.h"

namespace Phoinix
{
   // TODO: recommended code change from graphics drivers' devs
   // Driver developers recommend that you also store multiple buffers, like the vertex and index
   // buffer, into a single VkBuffer and use offsets in commands like vkCmdBindVertexBuffers
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