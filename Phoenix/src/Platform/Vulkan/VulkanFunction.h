#pragma once

#include "Graphics/Renderer/VulkanRenderer.h"
#include "VulkanImage.h"
#include "VulkanUniformBuffer.h"
#include "VulkanDescriptorSet.h"

namespace Phoenix::Graphics::Vulkan
{
  void MakeDefault()
  {
    Phoenix::VulkanDescriptorSet::MakeDefault();
    Phoenix::VulkanRenderer::MakeDefault();
    Phoenix::VulkanUniformBuffer::MakeDefault();
    Phoenix::VulkanImage::MakeDefault();
  }
}
