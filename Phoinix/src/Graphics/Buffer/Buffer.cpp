#include "Buffer.h"

namespace Phoinix
{
   uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice,
                           uint32_t typeFilter,
                           VkMemoryPropertyFlags properties)
   {
      VkPhysicalDeviceMemoryProperties memoryProperties;
      vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

      for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
      {
         if ((typeFilter & (1 << i)) &&
             (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
         {
            return i;
         }
      }

      ENGINE_ERR("Failed to find suitable memory");
      std::exit(-4);
   }

   void CreateBuffer(VulkanDevice& device,
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

      if (vkCreateBuffer(device.GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create vertex buffer");
         std::exit(-3);
      }

      // allocate memory
      VkMemoryRequirements memoryRequirements;
      vkGetBufferMemoryRequirements(device.GetDevice(), buffer, &memoryRequirements);

      VkMemoryAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memoryRequirements.size;
      allocInfo.memoryTypeIndex =
         FindMemoryType(device.GetPhysicalDevice(),
                        memoryRequirements.memoryTypeBits,
                        properties);

      if (vkAllocateMemory(device.GetDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to allocate memory to vertex buffer");
         std::exit(-3);
      }

      vkBindBufferMemory(device.GetDevice(), buffer, bufferMemory, 0);
   }
   
   void CopyBuffer(VulkanDevice& device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
   {
      VkCommandBufferAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      allocInfo.commandPool = device.GetCommandPool();
      allocInfo.commandBufferCount = 1;

      VkCommandBuffer commandBuffer;
      vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, &commandBuffer);

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

      vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
      vkQueueWaitIdle(device.GetGraphicsQueue());

      vkFreeCommandBuffers(device.GetDevice(), device.GetCommandPool(), 1, &commandBuffer);
   }
}