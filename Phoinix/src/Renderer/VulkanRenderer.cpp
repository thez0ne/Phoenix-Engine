#include "VulkanRenderer.h"

namespace Phoinix
{
   static bool RendererInitialized = false;

   Renderer* Renderer::Create(Window* window)
   {
      return new VulkanRenderer((VulkanWindow*)window);
   }

   VulkanRenderer::VulkanRenderer(VulkanWindow* window) :
      m_Window(window), m_Device(m_Instance.GetInstance(), (GLFWwindow*)window->GetWindow())
   {
      ENGINE_TRACE("Creating Renderer");
      CreatePipelineLayout();
      CreatePipeline();
      CreateCommandBuffer();
      CreateSyncObject();
      CreateDescriptorPool();
   }

   VulkanRenderer::~VulkanRenderer()
   {
      vkDeviceWaitIdle(m_Device.GetDevice());

      vkDestroyDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, nullptr);
      for (size_t i = 0; i < max_frames_in_flight; i++)
      {
         vkDestroySemaphore(m_Device.GetDevice(), m_RenderFinishedSemaphores[i], nullptr);
         vkDestroySemaphore(m_Device.GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
         vkDestroyFence(m_Device.GetDevice(), m_InFlightFences[i], nullptr);
      }
      vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
   }

   void VulkanRenderer::DrawFrame()
   {
      /*
         High level overview of rendering a frame
         - Wait for previous frame to finish
         - Acquire the next image from the swap chain
         - Record a command buffer to draw the scene into the image
         - Submit the recorded command buffer
         - Present the swap chain image to the screen
      */

      vkWaitForFences(
         m_Device.GetDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

      uint32_t imageIndex;
      VkResult result = vkAcquireNextImageKHR(m_Device.GetDevice(),
                                              m_Device.GetSwapChain(),
                                              UINT64_MAX,
                                              m_ImageAvailableSemaphores[m_CurrentFrame],
                                              VK_NULL_HANDLE,
                                              &imageIndex);

      if (result == VK_ERROR_OUT_OF_DATE_KHR)
      {
         m_SimplePipeline->RecreateSwapChain((GLFWwindow*)m_Window->GetWindow());
         return;
      }
      else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
      {
         ENGINE_ERR("Failed to get swap chain image");
         std::exit(-3);
      }

      vkResetFences(m_Device.GetDevice(), 1, &m_InFlightFences[m_CurrentFrame]);

      vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
      RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

      VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
      VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = waitSemaphores;
      submitInfo.pWaitDstStageMask = waitStages;

      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

      VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = signalSemaphores;

      if (vkQueueSubmit(
             m_Device.GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) !=
          VK_SUCCESS)
      {
         ENGINE_ERR("Failed to submit draw command");
         std::exit(-3);
      }

      VkPresentInfoKHR presentInfo{};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

      presentInfo.waitSemaphoreCount = 1;
      presentInfo.pWaitSemaphores = signalSemaphores;

      VkSwapchainKHR swapChains[] = {m_Device.GetSwapChain()};
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = swapChains;
      presentInfo.pImageIndices = &imageIndex;
      presentInfo.pResults =
         nullptr; // is used for checking status of individual swapchain presentation

      result = vkQueuePresentKHR(m_Device.GetPresentQueue(), &presentInfo);

      if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
          m_Window->m_FramebuffersResized)
      {
         m_Window->m_FramebuffersResized = false;
         m_SimplePipeline->RecreateSwapChain((GLFWwindow*)m_Window->GetWindow());
      }
      else if (result != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to present swap chain image");
         std::exit(-3);
      }

      m_CurrentFrame = (m_CurrentFrame + 1) % max_frames_in_flight;
   }

   //    void VulkanRenderer::Run()
   //    {
   //       while (!m_Window->ShouldClose())
   //       {
   //          glfwPollEvents();
   //          DrawFrame();
   //       }
   //       vkDeviceWaitIdle(m_Device.GetDevice());
   //    }

   void VulkanRenderer::CreatePipelineLayout()
   {
      // NOTE: used to set uniforms for the shaders
      VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
      pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      pipelineLayoutInfo.setLayoutCount = 0;
      pipelineLayoutInfo.pSetLayouts = nullptr;
      pipelineLayoutInfo.pushConstantRangeCount = 0;
      pipelineLayoutInfo.pPushConstantRanges = nullptr;

      if (vkCreatePipelineLayout(
             m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create pipeline layout");
         std::exit(-3);
      }
   }

   void VulkanRenderer::CreatePipeline()
   {
      auto pipelineConfig = VulkanPipeline::DefaultPipelineConfigInfo(m_Device.GetExtentWidth(),
                                                                      m_Device.GetExtentHeight());
      pipelineConfig.renderPass = m_Device.GetRenderPass();
      pipelineConfig.pipelineLayout = m_PipelineLayout;
      m_SimplePipeline = std::make_unique<VulkanPipeline>(
         m_Device, "shaders/simple.vert.spv", "shaders/simple.frag.spv", pipelineConfig);
   }

   void VulkanRenderer::CreateCommandBuffer()
   {
      m_CommandBuffers.resize(max_frames_in_flight);

      VkCommandBufferAllocateInfo allocationInfo{};
      allocationInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
      allocationInfo.commandPool = m_Device.GetCommandPool();
      allocationInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
      allocationInfo.commandBufferCount = 1;

      allocationInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

      if (vkAllocateCommandBuffers(
             m_Device.GetDevice(), &allocationInfo, m_CommandBuffers.data()) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to allocate command buffer");
         std::exit(-3);
      }
   }

   void VulkanRenderer::BeginRender()
   {
      vkWaitForFences(
         m_Device.GetDevice(), 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

      // uint32_t imageIndex;
      VkResult result = vkAcquireNextImageKHR(m_Device.GetDevice(),
                                              m_Device.GetSwapChain(),
                                              UINT64_MAX,
                                              m_ImageAvailableSemaphores[m_CurrentFrame],
                                              VK_NULL_HANDLE,
                                              &imageIndex);

      if (result == VK_ERROR_OUT_OF_DATE_KHR)
      {
         m_SimplePipeline->RecreateSwapChain((GLFWwindow*)m_Window->GetWindow());
         return;
      }

      if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
      {
         ENGINE_ERR("Failed to get swap chain image");
         std::exit(-3);
      }

      vkResetFences(m_Device.GetDevice(), 1, &m_InFlightFences[m_CurrentFrame]);

      vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);

      // NOTE: setup CommandBuffer and RenderPass to start rendering
      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = 0;
      beginInfo.pInheritanceInfo = nullptr;

      if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to begin recording command buffer");
         std::exit(-3);
      }

      VkRenderPassBeginInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassInfo.renderPass = m_Device.GetRenderPass();
      renderPassInfo.framebuffer = m_SimplePipeline->GetSwapChainFramebuffers()[imageIndex];
      renderPassInfo.renderArea.offset = {0, 0};
      renderPassInfo.renderArea.extent = m_Device.GetExtent();

      VkClearValue clearColour{{{0.0f, 0.0f, 0.0f, 1.0f}}};
      renderPassInfo.clearValueCount = 1;
      renderPassInfo.pClearValues = &clearColour;

      vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

      VkViewport viewport{};
      viewport.x = 0.0f;
      viewport.y = 0.0f;
      viewport.width = static_cast<float>(m_Device.GetExtentWidth());
      viewport.height = static_cast<float>(m_Device.GetExtentHeight());
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      vkCmdSetViewport(m_CommandBuffers[m_CurrentFrame], 0, 1, &viewport);

      VkRect2D scissor{};
      scissor.offset = {0, 0};
      scissor.extent = m_Device.GetExtent();
      vkCmdSetScissor(m_CommandBuffers[m_CurrentFrame], 0, 1, &scissor);
   }

   void VulkanRenderer::Render()
   {
      // NOTE: actual drawing of the geometry
      {
         m_SimplePipeline->Bind(m_CommandBuffers[m_CurrentFrame]);
         vkCmdDraw(m_CommandBuffers[m_CurrentFrame], 3, 1, 0, 0);
      }
   }

   void VulkanRenderer::EndRender()
   {
      // NOTE: cleanup of CommandBuffer and RenderPass
      vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);

      if (vkEndCommandBuffer(m_CommandBuffers[m_CurrentFrame]) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to end recording command buffer");
         std::exit(-3);
      }

      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

      VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
      VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = waitSemaphores;
      submitInfo.pWaitDstStageMask = waitStages;

      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

      VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = signalSemaphores;

      if (vkQueueSubmit(
             m_Device.GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) !=
          VK_SUCCESS)
      {
         ENGINE_ERR("Failed to submit draw command");
         std::exit(-3);
      }

      VkPresentInfoKHR presentInfo{};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

      presentInfo.waitSemaphoreCount = 1;
      presentInfo.pWaitSemaphores = signalSemaphores;

      VkSwapchainKHR swapChains[] = {m_Device.GetSwapChain()};
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = swapChains;
      presentInfo.pImageIndices = &imageIndex;
      presentInfo.pResults =
         nullptr; // is used for checking status of individual swapchain presentation

      VkResult result = vkQueuePresentKHR(m_Device.GetPresentQueue(), &presentInfo);

      if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
          m_Window->m_FramebuffersResized)
      {
         m_Window->m_FramebuffersResized = false;
         m_SimplePipeline->RecreateSwapChain((GLFWwindow*)m_Window->GetWindow());
      }
      else if (result != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to present swap chain image");
         std::exit(-3);
      }

      m_CurrentFrame = (m_CurrentFrame + 1) % max_frames_in_flight;
   }

   void VulkanRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
   {
      // NOTE: setup CommandBuffer and RenderPass to start rendering
      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = 0;
      beginInfo.pInheritanceInfo = nullptr;

      if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to begin recording command buffer");
         std::exit(-3);
      }

      VkRenderPassBeginInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassInfo.renderPass = m_Device.GetRenderPass();
      renderPassInfo.framebuffer = m_SimplePipeline->GetSwapChainFramebuffers()[imageIndex];
      renderPassInfo.renderArea.offset = {0, 0};
      renderPassInfo.renderArea.extent = m_Device.GetExtent();

      VkClearValue clearColour{{{0.0f, 0.0f, 0.0f, 1.0f}}};
      renderPassInfo.clearValueCount = 1;
      renderPassInfo.pClearValues = &clearColour;

      vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


      // NOTE: actual drawing of the geometry
      {
         m_SimplePipeline->Bind(commandBuffer);

         VkViewport viewport{};
         viewport.x = 0.0f;
         viewport.y = 0.0f;
         viewport.width = static_cast<float>(m_Device.GetExtentWidth());
         viewport.height = static_cast<float>(m_Device.GetExtentHeight());
         viewport.minDepth = 0.0f;
         viewport.maxDepth = 1.0f;
         vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

         VkRect2D scissor{};
         scissor.offset = {0, 0};
         scissor.extent = m_Device.GetExtent();
         vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
         vkCmdDraw(commandBuffer, 3, 1, 0, 0);
      }

      // NOTE: cleanup of CommandBuffer and RenderPass
      vkCmdEndRenderPass(commandBuffer);

      if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to end recording command buffer");
         std::exit(-3);
      }
   }

   void VulkanRenderer::CreateSyncObject()
   {
      m_ImageAvailableSemaphores.resize(max_frames_in_flight);
      m_RenderFinishedSemaphores.resize(max_frames_in_flight);
      m_InFlightFences.resize(max_frames_in_flight);

      VkSemaphoreCreateInfo semaphoreInfo{};
      semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

      VkFenceCreateInfo fenceInfo{};
      fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
      fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

      for (size_t i = 0; i < max_frames_in_flight; i++)
      {
         if (vkCreateSemaphore(
                m_Device.GetDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
             vkCreateSemaphore(
                m_Device.GetDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
             vkCreateFence(m_Device.GetDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]) !=
                VK_SUCCESS)
         {
            ENGINE_ERR("Failed to create semaphores");
            std::exit(-3);
         }
      }
   }

   void VulkanRenderer::CreateDescriptorPool()
   {
      std::vector<VkDescriptorPoolSize> pool_sizes = {
         {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

      VkDescriptorPoolCreateInfo pool_info{};
      pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
      pool_info.maxSets = 1000 * pool_sizes.size();
      pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
      pool_info.pPoolSizes = pool_sizes.data();

      if (vkCreateDescriptorPool(m_Device.GetDevice(), &pool_info, nullptr, &m_DescriptorPool) !=
          VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create descriptor pool");
         std::exit(-3);
      }
   }
}
