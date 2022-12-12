#include "VulkanPipeline.h"

#include "Vertex.h"

// TODO prob needs to be a part of the renderer

namespace Phoinix
{
   VulkanPipeline::VulkanPipeline(VulkanDevice& device,
                                  const std::string& vertFile,
                                  const std::string& fragFile,
                                  const PipelineConfigInfo& configInfo) :
      m_Device(device)
   {
      CreateGraphicsPipeline(vertFile, fragFile, configInfo);
      CreateFrameBuffers();
   }

   VulkanPipeline::~VulkanPipeline()
   {
      for (const auto framebuffer : m_SwapChainFrameBuffers)
      {
         vkDestroyFramebuffer(m_Device.GetDevice(), framebuffer, nullptr);
      }
      vkDestroyShaderModule(m_Device.GetDevice(), m_FragShaderModule, nullptr);
      vkDestroyShaderModule(m_Device.GetDevice(), m_VertShaderModule, nullptr);
      vkDestroyPipeline(m_Device.GetDevice(), m_GraphicsPipeline, nullptr);
   }

   std::vector<char> VulkanPipeline::ReadFile(const std::string& filePath)
   {
      std::ifstream file(filePath, std::ios::ate | std::ios::binary);
      if (!file.is_open())
      {
         ENGINE_ERR("File could not be opened with path: {}", filePath);
         std::exit(-3);
      }

      const size_t fileSize = static_cast<size_t>(file.tellg());
      std::vector<char> buffer(fileSize);

      file.seekg(0);
      file.read(buffer.data(), fileSize);

      file.close();
      return buffer;
   }

   void VulkanPipeline::CreateFrameBuffers()
   {
      auto imageViews = m_Device.GetSwapChainImageViews();
      m_SwapChainFrameBuffers.resize(imageViews.size());

      for (size_t i = 0; i < m_SwapChainFrameBuffers.size(); i++)
      {
         VkImageView attachment[] = {imageViews[i]};

         VkFramebufferCreateInfo framebufferInfo{};
         framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
         framebufferInfo.renderPass = m_Device.GetRenderPass();
         framebufferInfo.attachmentCount = 1;
         framebufferInfo.pAttachments = attachment;
         framebufferInfo.width = m_Device.GetExtentWidth();
         framebufferInfo.height = m_Device.GetExtentHeight();
         framebufferInfo.layers = 1;

         if (vkCreateFramebuffer(
                m_Device.GetDevice(), &framebufferInfo, nullptr, &m_SwapChainFrameBuffers[i]) !=
             VK_SUCCESS)
         {
            ENGINE_ERR("Failed to create framebuffer");
            std::exit(-3);
         }
      }
   }

   void VulkanPipeline::CreateGraphicsPipeline(const std::string& vertFilePath,
                                               const std::string& fragFilePath,
                                               const PipelineConfigInfo& configInfo)
   {
      const auto vertCode = ReadFile(vertFilePath);
      const auto fragCode = ReadFile(fragFilePath);

      CreateShaderModule(vertCode, &m_VertShaderModule);
      CreateShaderModule(fragCode, &m_FragShaderModule);

      VkPipelineShaderStageCreateInfo shaderStages[2];
      shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
      shaderStages[0].module = m_VertShaderModule;
      shaderStages[0].pName = "main";
      shaderStages[0].flags = 0;
      shaderStages[0].pNext = nullptr;
      shaderStages[0].pSpecializationInfo = nullptr;
      shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
      shaderStages[1].module = m_FragShaderModule;
      shaderStages[1].pName = "main";
      shaderStages[1].flags = 0;
      shaderStages[1].pNext = nullptr;
      shaderStages[1].pSpecializationInfo = nullptr;

      auto bindingDescriptions = Vertex::GetBindingDescription();
      auto attributeDescriptions = Vertex::GetAttributeDescription();

      VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
      vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertexInputInfo.vertexBindingDescriptionCount = 1;
      vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
      vertexInputInfo.pVertexBindingDescriptions = &bindingDescriptions;
      vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

      VkGraphicsPipelineCreateInfo pipelineInfo{};
      pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineInfo.stageCount = 2;
      pipelineInfo.pStages = shaderStages;
      pipelineInfo.pVertexInputState = &vertexInputInfo;
      pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
      pipelineInfo.pViewportState = &configInfo.viewportInfo;
      pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
      pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
      pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
      pipelineInfo.pDynamicState = nullptr;
      pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;

      pipelineInfo.layout = configInfo.pipelineLayout;
      pipelineInfo.renderPass = configInfo.renderPass;
      pipelineInfo.subpass = configInfo.subpass;

      pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
      pipelineInfo.basePipelineIndex = -1;

      // TODO: MOVE THIS TO CREATION OF PIPELINE LAYOUT??
      const std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                         VK_DYNAMIC_STATE_SCISSOR};

      VkPipelineDynamicStateCreateInfo dynamicState{};
      dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
      dynamicState.pDynamicStates = dynamicStates.data();

      pipelineInfo.pDynamicState = &dynamicState;

      if (vkCreateGraphicsPipelines(m_Device.GetDevice(),
                                    VK_NULL_HANDLE,
                                    1,
                                    &pipelineInfo,
                                    nullptr,
                                    &m_GraphicsPipeline) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create graphics pipeline");
         std::exit(-3);
      }
   }

   void VulkanPipeline::CreateShaderModule(const std::vector<char>& code,
                                           VkShaderModule* shaderModule) const
   {
      VkShaderModuleCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      createInfo.codeSize = code.size();
      createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

      if (vkCreateShaderModule(m_Device.GetDevice(), &createInfo, nullptr, shaderModule) !=
          VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create shader module");
         std::exit(-3);
      }
   }

   void VulkanPipeline::Bind(VkCommandBuffer commandBuffer)
   {
      vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
   }

   PipelineConfigInfo VulkanPipeline::DefaultPipelineConfigInfo(const uint32_t width,
                                                                const uint32_t height)
   {
      PipelineConfigInfo configInfo{};

      configInfo.inputAssemblyInfo.sType =
         VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

      // NOTE: viewport stretches the resulting image
      configInfo.viewport.x = 0.0f;
      configInfo.viewport.y = 0.0f;
      configInfo.viewport.width = static_cast<float>(width);
      configInfo.viewport.height = static_cast<float>(height);
      configInfo.viewport.minDepth = 0.0f;
      configInfo.viewport.maxDepth = 1.0f;

      // NOTE: scissor cuts off resulting image
      configInfo.scissor.offset = {0, 0};
      configInfo.scissor.extent = {width, height};

      configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      configInfo.viewportInfo.viewportCount = 1;
      configInfo.viewportInfo.pViewports = &configInfo.viewport;
      configInfo.viewportInfo.scissorCount = 1;
      configInfo.viewportInfo.pScissors = &configInfo.scissor;

      configInfo.rasterizationInfo.sType =
         VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
      configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
      configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
      configInfo.rasterizationInfo.lineWidth = 1.0f;
      configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE; // used for back face culling
      configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
      configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
      configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
      configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
      configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

      configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
      configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      configInfo.multisampleInfo.minSampleShading = 1.0f;
      configInfo.multisampleInfo.pSampleMask = nullptr;
      configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
      configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;

      configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
         VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
      configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
      configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
      configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
      configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

      // NOTE: possible blend code
      /*
      if (blendEnable) {
          finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor
      * oldColor.rgb); finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp>
      (dstAlphaBlendFactor * oldColor.a); } else { finalColor = newColor;
      }

      finalColor = finalColor & colorWriteMask;

      colorBlendAttachment.blendEnable = VK_TRUE;
      colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
      colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
      colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
      */

      configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
      configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_AND;
      configInfo.colorBlendInfo.attachmentCount = 1;
      configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
      configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
      configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
      configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
      configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

      configInfo.depthStencilInfo.sType =
         VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
      configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
      configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
      configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
      configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
      configInfo.depthStencilInfo.minDepthBounds = 0.0f;
      configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
      configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
      configInfo.depthStencilInfo.front = {};
      configInfo.depthStencilInfo.back = {};

      return configInfo;
   }

   void VulkanPipeline::RecreateSwapChain(GLFWwindow* window)
   {
      // NOTE: handles minimizing the window
      int width = 0, height = 0;
      glfwGetFramebufferSize(window, &width, &height);
      while (width == 0 || height == 0)
      {
         glfwGetFramebufferSize(window, &width, &height);
         glfwWaitEvents();
      }

      vkDeviceWaitIdle(m_Device.GetDevice());

      CleanupSwapChain();

      m_Device.CreateSwapChain();
      m_Device.CreateImageViews();
      CreateFrameBuffers();
   }

   void VulkanPipeline::CleanupSwapChain()
   {
      CleanupFrameBuffers();
      m_Device.CleanupSwapChain();
   }

   void VulkanPipeline::CleanupFrameBuffers()
   {
      for (size_t i = 0; i < m_SwapChainFrameBuffers.size(); i++)
      {
         vkDestroyFramebuffer(m_Device.GetDevice(), m_SwapChainFrameBuffers[i], nullptr);
      }
   }
}
