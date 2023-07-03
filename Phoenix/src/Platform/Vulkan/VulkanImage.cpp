#include "VulkanImage.h"

#include <stb_image_write.h>

// TODO temp
#include "Core/Application.h"
#include "Graphics/Renderer/VulkanRenderer.h"
#include "backends/imgui_impl_vulkan.h"

#include "Graphics/Buffer/Buffer.h"
#include "Platform/Vulkan/VulkanDevice.h"

namespace Phoenix
{
  VulkanImage::VulkanImage(uint32_t width, uint32_t height, Format format, void* data) :
    Image(width, height, format)
  {
    AllocateMemory(m_Width * m_Height * BytesPerPixel(m_Format));
    // TODO set initial data
    if (data)
    {
      SetData(data);
    }

    Release();
  }

  VulkanImage::~VulkanImage() {}

  void VulkanImage::SetData(const void* data)
  {
    size_t upload_size = m_Width * m_Height * BytesPerPixel(m_Format);

    // size_t alignedSize;
    if (!m_StagingBuffer)
    {
      // Create the Upload Buffer
      {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = upload_size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        VKASSERT(vkCreateBuffer(VulkanDevice::Device(), &bufferInfo, nullptr, &m_StagingBuffer),
                 "Failed to create image staging buffer");

        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(VulkanDevice::Device(), m_StagingBuffer, &req);
        m_AlignedSize = req.size;

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = req.size;
        allocInfo.memoryTypeIndex =
          FindMemoryType(req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        VKASSERT(
          vkAllocateMemory(VulkanDevice::Device(), &allocInfo, nullptr, &m_StagingBufferMemory),
          "Failed to allocate image staging buffer");
        VKASSERT(
          vkBindBufferMemory(VulkanDevice::Device(), m_StagingBuffer, m_StagingBufferMemory, 0),
          "Failed to bind image buffer");
      }
    }

    // Upload to Buffer
    {
      vkMapMemory(VulkanDevice::Device(), m_StagingBufferMemory, 0, m_AlignedSize, 0, &m_Data);

      memcpy(m_Data, data, upload_size);
      VkMappedMemoryRange range[1] = {};
      range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
      range[0].memory = m_StagingBufferMemory;
      range[0].size = m_AlignedSize;
      VKASSERT(vkFlushMappedMemoryRanges(VulkanDevice::Device(), 1, range),
               "Failed to flush image staging buffer");

      vkUnmapMemory(VulkanDevice::Device(), m_StagingBufferMemory);
    }

    // Copy to Image
    {
      VkCommandBuffer command_buffer = VulkanRenderer::GetTempCommandBuffer();

      VkImageMemoryBarrier copyBarrier{};
      copyBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      copyBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      copyBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      copyBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      copyBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      copyBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      copyBarrier.image = m_Image;
      copyBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      copyBarrier.subresourceRange.levelCount = 1;
      copyBarrier.subresourceRange.layerCount = 1;
      vkCmdPipelineBarrier(command_buffer,
                           VK_PIPELINE_STAGE_HOST_BIT,
                           VK_PIPELINE_STAGE_TRANSFER_BIT,
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           1,
                           &copyBarrier);

      VkBufferImageCopy region{};
      region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      region.imageSubresource.layerCount = 1;
      region.imageExtent.width = m_Width;
      region.imageExtent.height = m_Height;
      region.imageExtent.depth = 1;
      vkCmdCopyBufferToImage(
        command_buffer, m_StagingBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

      VkImageMemoryBarrier useBarrier{};
      useBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      useBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      useBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      useBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      useBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      useBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      useBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      useBarrier.image = m_Image;
      useBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      useBarrier.subresourceRange.levelCount = 1;
      useBarrier.subresourceRange.layerCount = 1;
      vkCmdPipelineBarrier(command_buffer,
                           VK_PIPELINE_STAGE_TRANSFER_BIT,
                           VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                           0,
                           0,
                           nullptr,
                           0,
                           nullptr,
                           1,
                           &useBarrier);

      VulkanRenderer::FlushTempCommandBuffer(command_buffer);
    }
  }

  void VulkanImage::Resize(uint32_t width, uint32_t height)
  {
    if (m_Image && m_Width == width && m_Height == height)
      return;

    m_Width = width;
    m_Height = height;

    VulkanRenderer::RegisterInstantCleanup([sampler = m_Sampler,
                                            imageView = m_ImageView,
                                            image = m_Image,
                                            memory = m_Memory,
                                            stagingBuffer = m_StagingBuffer,
                                            stagingBufferMemory = m_StagingBufferMemory]() {
      VkDevice device = VulkanDevice::Device();

      vkDestroySampler(device, sampler, nullptr);
      vkDestroyImageView(device, imageView, nullptr);
      vkDestroyImage(device, image, nullptr);
      vkFreeMemory(device, memory, nullptr);
      vkDestroyBuffer(device, stagingBuffer, nullptr);
      vkFreeMemory(device, stagingBufferMemory, nullptr);
    });

    m_Sampler = nullptr;
    m_ImageView = nullptr;
    m_Image = nullptr;
    m_Memory = nullptr;
    m_StagingBuffer = nullptr;
    m_StagingBufferMemory = nullptr;
    AllocateMemory(m_Width * m_Height * BytesPerPixel(m_Format));
  }

  void VulkanImage::Save(const std::string& filepath)
  {
    stbi_write_png(filepath.c_str(), m_Width, m_Height, 4, m_Data, 0);
  }

  void VulkanImage::Release()
  {
    VulkanRenderer::RegisterCleanup([this]() {
      VkDevice device = VulkanDevice::Device();

      vkDestroySampler(device, m_Sampler, nullptr);
      vkDestroyImageView(device, m_ImageView, nullptr);
      vkDestroyImage(device, m_Image, nullptr);
      vkFreeMemory(device, m_Memory, nullptr);
      vkDestroyBuffer(device, m_StagingBuffer, nullptr);
      vkFreeMemory(device, m_StagingBufferMemory, nullptr);
    });
  }

  void VulkanImage::AllocateMemory(uint64_t size)
  {
    VkFormat vkFormat = FormatToVulkanFormat(m_Format);
    // create image
    {
      VkImageCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      createInfo.imageType = VK_IMAGE_TYPE_2D;
      createInfo.format = vkFormat;
      createInfo.extent.width = m_Width;
      createInfo.extent.height = m_Height;
      createInfo.extent.depth = 1;
      createInfo.mipLevels = 1;
      createInfo.arrayLayers = 1;
      createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
      createInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
      createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      createInfo.flags = 0;
      VKASSERT(vkCreateImage(VulkanDevice::Device(), &createInfo, nullptr, &m_Image),
               "Failed to create image");

      VkMemoryRequirements memoryRequirements{};
      vkGetImageMemoryRequirements(VulkanDevice::Device(), m_Image, &memoryRequirements);

      VkMemoryAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
      allocInfo.allocationSize = memoryRequirements.size;
      allocInfo.memoryTypeIndex =
        FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      VKASSERT(vkAllocateMemory(VulkanDevice::Device(), &allocInfo, nullptr, &m_Memory),
               "Failed to allocate memory");

      VKASSERT(vkBindImageMemory(VulkanDevice::Device(), m_Image, m_Memory, 0),
               "Failed to bind memory of image");
    }

    // Create image view
    {
      VkImageViewCreateInfo viewInfo{};
      viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      viewInfo.image = m_Image;
      viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      viewInfo.format = vkFormat;
      viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      viewInfo.subresourceRange.levelCount = 1;
      viewInfo.subresourceRange.layerCount = 1;
      VKASSERT(vkCreateImageView(VulkanDevice::Device(), &viewInfo, nullptr, &m_ImageView),
               "Failed to create image view");
    }

    // Create sampler
    {
      VkSamplerCreateInfo samplerInfo{};
      samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
      samplerInfo.magFilter = VK_FILTER_LINEAR;
      samplerInfo.minFilter = VK_FILTER_LINEAR;
      samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
      samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
      samplerInfo.minLod = -1000;
      samplerInfo.maxLod = 1000;
      // anisotropic filtering - currently off
      samplerInfo.anisotropyEnable = VK_FALSE;
      samplerInfo.maxAnisotropy = 1.0f;
      // color to use if using clamp border
      samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
      // samplerInfo.unnormalizedCoordinates = VK_FALSE; // normalized tex coords

      VKASSERT(vkCreateSampler(VulkanDevice::Device(), &samplerInfo, nullptr, &m_Sampler),
               "Failed to create sampler");
    }

    // TODO is it possible to not use the imgui function for this
    m_DescriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(
      m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  }

  void VulkanImage::MakeDefault()
  {
    CreateFunc = CreateFuncVulkan;
  }

  Image* VulkanImage::CreateFuncVulkan(uint32_t width, uint32_t height, Format format, void* data)
  {
    return new VulkanImage(width, height, format, data);
  }
}
