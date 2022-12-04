#pragma once
#include "pch.h"

#include "Core/Renderer.h"
#include "Platform/Vulkan/VulkanInstance.h"
#include "Platform/Vulkan/VulkanDevice.h"
#include "Platform/Vulkan/VulkanPipeline.h"

namespace Phoinix
{
   class VulkanRenderer : public Renderer
   {
    public:
      static constexpr int width = 800;
      static constexpr int height = 600;
      const int max_frames_in_flight = 2;

      VulkanRenderer(VulkanWindow* window);
      ~VulkanRenderer();

      VulkanRenderer(const VulkanRenderer&) = delete;
      VulkanRenderer& operator=(const VulkanRenderer&) = delete;
      VulkanRenderer(VulkanRenderer&&) = delete;
      VulkanRenderer& operator=(VulkanRenderer&&) = delete;

      virtual void DrawFrame() override;

    private:
      VulkanWindow* m_Window;
      VulkanInstance m_Instance{}; // TODO temp, might move to window class if this is per window
      VulkanDevice m_Device;
      VkPipelineLayout m_PipelineLayout{};
      std::unique_ptr<VulkanPipeline> m_SimplePipeline;

      uint32_t m_CurrentFrame = 0;
      std::vector<VkCommandBuffer> m_CommandBuffers;

      std::vector<VkSemaphore> m_ImageAvailableSemaphores;
      std::vector<VkSemaphore> m_RenderFinishedSemaphores;
      std::vector<VkFence> m_InFlightFences;

      void CreatePipelineLayout();
      void CreatePipeline();
      void CreateCommandBuffer();
      void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
      void CreateSyncObject();
   };
}
