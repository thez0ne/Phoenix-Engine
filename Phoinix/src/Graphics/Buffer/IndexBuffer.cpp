#include "IndexBuffer.h"

namespace Phoinix
{
   IndexBuffer::IndexBuffer(VulkanDevice& device, std::vector<uint16_t> indices) : m_Device(device)
   {
      VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

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
      memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
      vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);

      CreateBuffer(m_Device,
                   bufferSize,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                   m_IndexBuffer,
                   m_IndexBufferMemory);
      CopyBuffer(m_Device, stagingBuffer, m_IndexBuffer, bufferSize);

      vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
      vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
   }

   IndexBuffer::~IndexBuffer() 
   {
      vkDestroyBuffer(m_Device.GetDevice(), m_IndexBuffer, nullptr);
      vkFreeMemory(m_Device.GetDevice(), m_IndexBufferMemory, nullptr);
   }
   
   void IndexBuffer::Bind(VkCommandBuffer commandBuffer) 
   {
      vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
   }
}