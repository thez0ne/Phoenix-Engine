#pragma once

#include "Graphics/Renderer/VulkanRenderer.h"
#include "VulkanImage.h"
#include "VulkanUniformBuffer.h"
#include "VulkanDescriptorSet.h"

namespace Phoinix::Graphics::Vulkan
{
   void MakeDefault()
   {
      Phoinix::VulkanDescriptorSet::MakeDefault();
      Phoinix::VulkanRenderer::MakeDefault();
      Phoinix::VulkanUniformBuffer::MakeDefault();
      Phoinix::VulkanImage::MakeDefault();
   }
}
