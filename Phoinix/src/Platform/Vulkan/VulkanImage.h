#pragma once
#include "pch.h"

#include <imgui.h>

#include "Graphics/Image.h"
#include <vulkan/vulkan.h>

namespace Phoinix
{
   static VkFormat FormatToVulkanFormat(Format format)
   {
      switch (format)
      {
         case Format::NONE:
            PHOINIX_ASSERT(nullptr, "Invalid image format given");
         case Format::RGBA:
            return VK_FORMAT_R8G8B8A8_UNORM;
         case Format::RGBA32F:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
         default:
            return static_cast<VkFormat>(0);
      }
   }

   class VulkanImage : public Image
   {
    public:
      VulkanImage(uint32_t width, uint32_t height, Format format, void* data = nullptr);
      virtual ~VulkanImage();

      virtual void SetData(const void* data) override;
      virtual void ImGuiBind() override
      {
         ImGui::Image(
            m_DescriptorSet, {(float)GetWidth(), (float)GetHeight()}, ImVec2(0, 1), ImVec2(1, 0));
      }

      virtual void* GetDescriptorSet() override { return m_DescriptorSet; }
      virtual void Resize(uint32_t width, uint32_t height) override;

      static void MakeDefault();

    protected:
      static Image* CreateFuncVulkan(uint32_t width, uint32_t height, Format format, void* data);

    private:
      VkImage m_Image;
      VkImageView m_ImageView;
      VkDeviceMemory m_Memory;
      VkSampler m_Sampler;

      // TODO temp
      VkDescriptorSet m_DescriptorSet;
      VkBuffer m_StagingBuffer = nullptr;
      VkDeviceMemory m_StagingBufferMemory;
      size_t m_AlignedSize;

      void Release();
      void AllocateMemory(uint64_t size);
   };
}
