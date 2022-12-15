#include "Buffer.h"
#include "Core/Core.h"

namespace Phoinix
{
   // TODO: should these methods be a part of device?
   uint32_t FindMemoryType(/*const VkPhysicalDevice& physicalDevice,*/
                           uint32_t typeFilter,
                           VkMemoryPropertyFlags properties)
   {
      VkPhysicalDeviceMemoryProperties memoryProperties;
      vkGetPhysicalDeviceMemoryProperties(VulkanDevice::PhysicalDevice(), &memoryProperties);

      for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
      {
         if ((typeFilter & (1 << i)) &&
             (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
         {
            return i;
         }
      }

      VKASSERT(5, "Failed to find suitable memory");
      return 999999;
   }

   void CreateBuffer(/*VulkanDevice& device,*/
                     VkDeviceSize size,
                     VkBufferUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VkBuffer& buffer,
                     VkDeviceMemory& bufferMemory)
   {
      // create buffer
      VkBufferCreateInfo bufferInfo{};
      bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      bufferInfo.size = size;
      bufferInfo.usage = usage;
      bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

      VKASSERT(vkCreateBuffer(VulkanDevice::Device(), &bufferInfo, nullptr, &buffer), "Failed to create vertex buffer");

      // allocate memory
      VkMemoryRequirements memoryRequirements;
      vkGetBufferMemoryRequirements(VulkanDevice::Device(), buffer, &memoryRequirements);

      VkMemoryAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memoryRequirements.size;
      allocInfo.memoryTypeIndex =
         FindMemoryType(memoryRequirements.memoryTypeBits,
                        properties);

      VKASSERT(vkAllocateMemory(VulkanDevice::Device(), &allocInfo, nullptr, &bufferMemory), "Failed to allocate memory to vertex buffer");

      vkBindBufferMemory(VulkanDevice::Device(), buffer, bufferMemory, 0);
   }
   
   void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
   {
      VkCommandBufferAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      allocInfo.commandPool = VulkanDevice::CommandPool();
      allocInfo.commandBufferCount = 1;

      VkCommandBuffer commandBuffer;
      vkAllocateCommandBuffers(VulkanDevice::Device(), &allocInfo, &commandBuffer);

      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

      vkBeginCommandBuffer(commandBuffer, &beginInfo);

      VkBufferCopy copyRegion{};
      copyRegion.srcOffset = 0;
      copyRegion.dstOffset = 0;
      copyRegion.size = size;
      vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

      vkEndCommandBuffer(commandBuffer);
      
      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &commandBuffer;

      vkQueueSubmit(VulkanDevice::GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
      vkQueueWaitIdle(VulkanDevice::GraphicsQueue());

      vkFreeCommandBuffers(VulkanDevice::Device(), VulkanDevice::CommandPool(), 1, &commandBuffer);
   }
}