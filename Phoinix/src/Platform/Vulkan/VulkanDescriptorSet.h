#pragma once
#include "pch.h"

#include <vulkan/vulkan.h>
#include "Graphics/API/DescriptorSet.h"

namespace Phoinix
{
   class VulkanDescriptorSet : public DescriptorSet
   {
    public:
      VulkanDescriptorSet();
      virtual ~VulkanDescriptorSet();

      VkDescriptorSetLayout* GetDescriptorSetLayout() { return &m_DescriptorSetLayout; }

      static void MakeDefault();

    protected:
      static DescriptorSet* CreateFuncVulkan();

    private:
      VkDescriptorSetLayout m_DescriptorSetLayout;
   };
}
