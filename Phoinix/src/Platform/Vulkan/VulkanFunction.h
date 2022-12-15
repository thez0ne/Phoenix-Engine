#pragma once

#include "Graphics/Renderer/VulkanRenderer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanDescriptorSet.h"

namespace Phoinix::Graphics::Vulkan
{
   void MakeDefault()
   {
      Phoinix::VulkanDescriptorSet::MakeDefault();
      Phoinix::VulkanRenderer::MakeDefault();
      Phoinix::VulkanUniformBuffer::MakeDefault();
   }
}