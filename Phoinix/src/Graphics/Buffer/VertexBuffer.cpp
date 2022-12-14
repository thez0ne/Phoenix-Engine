#include "VertexBuffer.h"

#include "Core/Core.h"

namespace Phoinix
{
   VertexBuffer::VertexBuffer(VulkanDevice& device, std::vector<Vertex> vertices) : m_Device(device)
   {
      VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

      VkBuffer stagingBuffer;
      VkDeviceMemory stagingBufferMemory;

      CreateBuffer(m_Device,
                   bufferSize,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                   stagingBuffer,
                   stagingBufferMemory);

      void* data;
      vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
      memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
      vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);

      CreateBuffer(m_Device,
                   bufferSize,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                   m_VertexBuffer,
                   m_VertexBufferMemory);

      CopyBuffer(m_Device, stagingBuffer, m_VertexBuffer, bufferSize);
      vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
      vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
   }

   VertexBuffer::~VertexBuffer()
   {
      // TODO cleanup swap chain?
      vkDestroyBuffer(m_Device.GetDevice(), m_VertexBuffer, nullptr);
      vkFreeMemory(m_Device.GetDevice(), m_VertexBufferMemory, nullptr);
   }

   void VertexBuffer::Bind(VkCommandBuffer commandBuffer)
   {
      VkBuffer vertexBuffers[] = {m_VertexBuffer};
      VkDeviceSize offsets[] = {0};
      vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
   }
}