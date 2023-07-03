#include "VulkanDescriptorSet.h"

#include "Platform/Vulkan/VulkanDevice.h"

namespace Phoenix
{
  VulkanDescriptorSet::VulkanDescriptorSet()
  {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    // TODO make this more generic
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // NOTE: with multiple can use for skeletal animation
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    // TODO setup for image based descriptors
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    VKASSERT(vkCreateDescriptorSetLayout(
               VulkanDevice::Device(), &layoutInfo, nullptr, &m_DescriptorSetLayout),
             "Failed to create descriptor set layout");
  }

  VulkanDescriptorSet::~VulkanDescriptorSet()
  {
    vkDestroyDescriptorSetLayout(VulkanDevice::Device(), m_DescriptorSetLayout, nullptr);
  }

  void VulkanDescriptorSet::MakeDefault()
  {
    CreateFunc = CreateFuncVulkan;
  }

  DescriptorSet* VulkanDescriptorSet::CreateFuncVulkan()
  {
    return new VulkanDescriptorSet();
  }
}
