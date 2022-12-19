#pragma once
#include "pch.h"

#include "Vertex.h"
#include "Buffer.h"

namespace Phoinix
{
   class VertexBuffer
   {
    public:
      VertexBuffer(VulkanDevice& device, std::vector<Vertex> vertices);
      ~VertexBuffer();

      void Bind(VkCommandBuffer commandBuffer);

    private:
      VkBuffer m_VertexBuffer;
      VkDeviceMemory m_VertexBufferMemory;
      void* m_Data;
   };
}
