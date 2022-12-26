#pragma once
#include "pch.h"

#include <vulkan/vulkan.h>
#include "Graphics/API/UniformBuffer.h"

namespace Phoinix
{
  class VulkanUniformBuffer : public UniformBuffer
  {
  public:
    VulkanUniformBuffer(const UniformBufferObject& data);
    VulkanUniformBuffer();
    virtual ~VulkanUniformBuffer();

    void Update(uint32_t currentFrame);

    static void MakeDefault();

    [[nodiscard]] const VkBuffer& GetBuffer() const { return m_UniformBuffer; }

  protected:
    static UniformBuffer* CreateDataFuncVulkan(const UniformBufferObject& data);
    static UniformBuffer* CreateFuncVulkan();

  private:
    VkBuffer m_UniformBuffer;
    VkDeviceMemory m_UniformBufferMemory;
    void* m_Data;

    UniformBufferObject m_UBO;
  };
}
