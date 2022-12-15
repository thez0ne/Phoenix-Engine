#include "Graphics/Renderer/VulkanRenderer.h"

namespace Phoinix
{
   VulkanRenderer::VulkanRenderer(GLFWWindow* window) :
      m_Window(window), m_Device(m_Instance.GetInstance(), (GLFWwindow*)window->GetWindow())
   {
      ENGINE_TRACE("Creating Renderer");

      m_DescriptorSet = DescriptorSet::Create();
      CreatePipelineLayout();
      CreatePipeline();

      uniformBuffers.resize(max_frames_in_flight);
      for (size_t i = 0; i < max_frames_in_flight; i++)
      {
         uniformBuffers[i] = UniformBuffer::Create();
      }

      CreateCommandBuffer();
      CreateSyncObject();
      CreateDescriptorPool();
      CreateDescriptorSet();
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

      for (size_t i = 0; i < max_frames_in_flight; i++)
      {
         delete uniformBuffers[i];
      }

      delete m_DescriptorSet;
   }

   void VulkanRenderer::CreatePipelineLayout()
   {
      // NOTE: used to set uniforms for the shaders
      VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
      pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      // pipelineLayoutInfo.setLayoutCount = 0;
      pipelineLayoutInfo.setLayoutCount = 1;
      // pipelineLayoutInfo.pSetLayouts = nullptr;
      // auto d = ((VulkanDescriptorSet*)m_DescriptorSetLayout)->GetDescriptorSetLayout();
      pipelineLayoutInfo.pSetLayouts =
         ((VulkanDescriptorSet*)m_DescriptorSet)->GetDescriptorSetLayout();
      pipelineLayoutInfo.pushConstantRangeCount = 0;
      pipelineLayoutInfo.pPushConstantRanges = nullptr;

      VKASSERT(vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout), "Failed to create pipeline layout");
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

      VKASSERT(vkAllocateCommandBuffers(m_Device.GetDevice(), &allocationInfo, m_CommandBuffers.data()), "Failed to allocate command buffer");
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

      PHOINIX_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to get swap chain image");

      uniformBuffers[m_CurrentFrame]->Update(m_CurrentFrame);


      vkResetFences(m_Device.GetDevice(), 1, &m_InFlightFences[m_CurrentFrame]);

      vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);

      // NOTE: setup CommandBuffer and RenderPass to start rendering
      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
      beginInfo.flags = 0;
      beginInfo.pInheritanceInfo = nullptr;

      VKASSERT(vkBeginCommandBuffer(m_CommandBuffers[m_CurrentFrame], &beginInfo), "Failed to begin recording command buffer");

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
      /*
         High level overview of rendering a frame
         - Wait for previous frame to finish
         - Acquire the next image from the swap chain
         - Record a command buffer to draw the scene into the image
         - Submit the recorded command buffer
         - Present the swap chain image to the screen
      */
      // NOTE: actual drawing of the geometry
      {
         m_SimplePipeline->Bind(m_CommandBuffers[m_CurrentFrame]);
         temp.Bind(m_CommandBuffers[m_CurrentFrame]);
         temp2.Bind(m_CommandBuffers[m_CurrentFrame]);
         vkCmdBindDescriptorSets(m_CommandBuffers[m_CurrentFrame],
                                 VK_PIPELINE_BIND_POINT_GRAPHICS,
                                 m_PipelineLayout,
                                 0,
                                 1,
                                 &m_DescriptorSets[m_CurrentFrame],
                                 0,
                                 nullptr);
         // vkCmdDraw(m_CommandBuffers[m_CurrentFrame], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
         vkCmdDrawIndexed(m_CommandBuffers[m_CurrentFrame], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
      }
   }

   void VulkanRenderer::EndRender()
   {
      // NOTE: cleanup of CommandBuffer and RenderPass
      vkCmdEndRenderPass(m_CommandBuffers[m_CurrentFrame]);

      VKASSERT(vkEndCommandBuffer(m_CommandBuffers[m_CurrentFrame]), "Failed to end recording command buffer");

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

      VKASSERT(vkQueueSubmit(m_Device.GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]), "Failed to submit draw command");

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
      else VKASSERT(result, "Failed to present swap chain image");

      m_CurrentFrame = (m_CurrentFrame + 1) % max_frames_in_flight;
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
         VKASSERT(vkCreateSemaphore(m_Device.GetDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]), "Failed to create semaphores");
         VKASSERT(vkCreateSemaphore(m_Device.GetDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]), "Failed to create semaphores");
         VKASSERT(vkCreateFence(m_Device.GetDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]), "Failed to create semaphores");
      }
   }

   void VulkanRenderer::CreateDescriptorPool()
   {
      /*std::vector<VkDescriptorPoolSize> pool_sizes = {
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

      VKASSERT(vkCreateDescriptorPool(m_Device.GetDevice(), &pool_info, nullptr, &m_DescriptorPool), "Failed to create descriptor pool");*/

      VkDescriptorPoolSize poolSize{};
      poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      poolSize.descriptorCount = static_cast<uint32_t>(max_frames_in_flight);

      VkDescriptorPoolCreateInfo poolInfo{};
      poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      poolInfo.poolSizeCount = 1;
      poolInfo.pPoolSizes = &poolSize;
      poolInfo.maxSets = static_cast<uint32_t>(max_frames_in_flight);

      VKASSERT(vkCreateDescriptorPool(m_Device.GetDevice(), &poolInfo, nullptr, &m_DescriptorPool),
               "Failed to create descriptor pool");
   }

   void VulkanRenderer::CreateDescriptorSet()
   {
      // TODO might be able to be moved into VulkanDescriptorSet
      auto setLayout = ((VulkanDescriptorSet*)m_DescriptorSet)->GetDescriptorSetLayout();
      std::vector<VkDescriptorSetLayout> layouts(max_frames_in_flight, *setLayout);

      VkDescriptorSetAllocateInfo allocInfo{};
      allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
      allocInfo.descriptorPool = m_DescriptorPool;
      allocInfo.descriptorSetCount = static_cast<uint32_t>(max_frames_in_flight);
      allocInfo.pSetLayouts = layouts.data();

      m_DescriptorSets.resize(max_frames_in_flight);
      VKASSERT(vkAllocateDescriptorSets(m_Device.GetDevice(), &allocInfo, m_DescriptorSets.data()), "Failed to allocate descriptor set");

      
      for (size_t i = 0; i < max_frames_in_flight; i++)
      {
         VkDescriptorBufferInfo bufferInfo{};
         bufferInfo.buffer = static_cast<VulkanUniformBuffer*>(uniformBuffers[i])->GetBuffer();
         bufferInfo.offset = 0;
         bufferInfo.range = sizeof(UniformBufferObject);

         VkWriteDescriptorSet descriptorWrite{};
         descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
         descriptorWrite.dstSet = m_DescriptorSets[i];
         descriptorWrite.dstBinding = 0;
         descriptorWrite.dstArrayElement = 0;
         descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
         descriptorWrite.descriptorCount = 1;
         descriptorWrite.pBufferInfo = &bufferInfo;
         descriptorWrite.pImageInfo = nullptr;
         descriptorWrite.pTexelBufferView = nullptr;

         vkUpdateDescriptorSets(m_Device.GetDevice(), 1, &descriptorWrite, 0, nullptr);
      }
   }


   void VulkanRenderer::MakeDefault()
   {
      CreateFunc = CreateFuncVulkan;
   }

   Renderer* VulkanRenderer::CreateFuncVulkan(Window* window)
   {
      return new VulkanRenderer((GLFWWindow*)window);
   }
}
