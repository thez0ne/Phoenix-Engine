#include "VulkanUniformBuffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

namespace Phoenix
{
  VulkanUniformBuffer::VulkanUniformBuffer(const UniformBufferObject& data) : m_UBO(data)
  {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    CreateBuffer(bufferSize,
                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 m_UniformBuffer,
                 m_UniformBufferMemory);

    vkMapMemory(VulkanDevice::Device(), m_UniformBufferMemory, 0, bufferSize, 0, &m_Data);
  }

  VulkanUniformBuffer::VulkanUniformBuffer()
  {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    CreateBuffer(bufferSize,
                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 m_UniformBuffer,
                 m_UniformBufferMemory);

    vkMapMemory(VulkanDevice::Device(), m_UniformBufferMemory, 0, bufferSize, 0, &m_Data);
  }

  VulkanUniformBuffer::~VulkanUniformBuffer()
  {
    vkDestroyBuffer(VulkanDevice::Device(), m_UniformBuffer, nullptr);
    vkFreeMemory(VulkanDevice::Device(), m_UniformBufferMemory, nullptr);
  }

  void VulkanUniformBuffer::Update(uint32_t currentFrame)
  {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time =
      std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model =
      glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(
      glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.projection = glm::perspective(glm::radians(45.0f),
                                      VulkanDevice::GetInstance()->GetExtentWidth() /
                                        (float)VulkanDevice::GetInstance()->GetExtentHeight(),
                                      0.1f,
                                      10.0f);
    ubo.projection[1][1] *= -1;

    memcpy(m_Data, &ubo, sizeof(ubo));
  }

  void VulkanUniformBuffer::MakeDefault()
  {
    CreateFunc = CreateFuncVulkan;
    CreateDataFunc = CreateDataFuncVulkan;
  }

  UniformBuffer* VulkanUniformBuffer::CreateDataFuncVulkan(const UniformBufferObject& data)
  {
    return new VulkanUniformBuffer(data);
  }

  UniformBuffer* VulkanUniformBuffer::CreateFuncVulkan()
  {
    return new VulkanUniformBuffer();
  }
}
