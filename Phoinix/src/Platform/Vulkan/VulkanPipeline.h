#pragma once
#include "pch.h"

#include "VulkanDevice.h"

namespace Phoinix
{
   struct PipelineConfigInfo
   {
      VkViewport viewport;
      VkRect2D scissor;
      VkPipelineViewportStateCreateInfo viewportInfo;
      VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
      VkPipelineRasterizationStateCreateInfo rasterizationInfo;
      VkPipelineMultisampleStateCreateInfo multisampleInfo;
      VkPipelineColorBlendAttachmentState colorBlendAttachment;
      VkPipelineColorBlendStateCreateInfo colorBlendInfo;
      VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
      VkPipelineLayout pipelineLayout = nullptr;
      VkRenderPass renderPass = nullptr;
      uint32_t subpass = 0;
   };

   class VulkanPipeline
   {
    public:
      VulkanPipeline(VulkanDevice& device,
                     const std::string& vertFile,
                     const std::string& fragFile,
                     const PipelineConfigInfo& configInfo);
      ~VulkanPipeline();

      VulkanPipeline(const VulkanPipeline&) = delete;
      void operator=(const VulkanPipeline&) = delete;
      VulkanPipeline(VulkanPipeline&&) = delete;
      VulkanPipeline& operator=(VulkanPipeline&&) = delete;

      void Bind(VkCommandBuffer commandBuffer);

      static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);

      [[nodiscard]] std::vector<VkFramebuffer> GetSwapChainFramebuffers() const
      {
         return m_SwapChainFrameBuffers;
      }
      [[nodiscard]] const VkPipeline& GetVkPipeline() const { return m_GraphicsPipeline; }

      void RecreateSwapChain(GLFWwindow* window);

    private:
      VulkanDevice& m_Device;
      VkPipeline m_GraphicsPipeline{};
      VkShaderModule m_VertShaderModule{};
      VkShaderModule m_FragShaderModule{};
      std::vector<VkFramebuffer> m_SwapChainFrameBuffers;

      static std::vector<char> ReadFile(const std::string& filePath);

      void CreateGraphicsPipeline(const std::string& vertFilePath,
                                  const std::string& fragFilePath,
                                  const PipelineConfigInfo& configInfo);
      void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) const;
      void CreateFrameBuffers();

      void CleanupSwapChain();
      void CleanupFrameBuffers();
   };
}
