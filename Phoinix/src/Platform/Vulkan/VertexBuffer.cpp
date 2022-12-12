#include "VertexBuffer.h"

namespace Phoinix
{
    VertexBuffer::VertexBuffer(VulkanDevice& device, std::vector<Vertex> vertices) : m_Device(device)
    {
        // create buffer
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(vertices[0]) * vertices.size();
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if(vkCreateBuffer(m_Device.GetDevice(), &bufferInfo, nullptr, &m_VertexBuffer) != VK_SUCCESS)
        {
            ENGINE_ERR("Failed to create vertex buffer");
            std::exit(-3);
        }

        // allocate memory
        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(m_Device.GetDevice(), m_VertexBuffer, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if(vkAllocateMemory(m_Device.GetDevice(), &allocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS)
        {
            ENGINE_ERR("Failed to allocate memory to vertex buffer");
            std::exit(-3);
        }

        vkBindBufferMemory(m_Device.GetDevice(), m_VertexBuffer, m_VertexBufferMemory, 0);

        // Fill vertex buffer with data
        vkMapMemory(m_Device.GetDevice(), m_VertexBufferMemory, 0, bufferInfo.size, 0, &m_Data);
        memcpy(m_Data, vertices.data(), static_cast<size_t>(bufferInfo.size));
        vkUnmapMemory(m_Device.GetDevice(), m_VertexBufferMemory);
    }

    VertexBuffer::~VertexBuffer()
    {
        // WARNING cleanup swap chain?
        vkDestroyBuffer(m_Device.GetDevice(), m_VertexBuffer, nullptr);
        vkFreeMemory(m_Device.GetDevice(), m_VertexBufferMemory, nullptr);
    }

    void VertexBuffer::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer vertexBuffers[] = {m_VertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }

    uint32_t VertexBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(m_Device.GetPhysicalDevice(), &memoryProperties);

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        ENGINE_ERR("Failed to find suitable memory");
        std::exit(-4);
    }
}