#pragma once
#include "pch.h"

#include "Graphics/API/Renderer.h"
#include "Platform/GLFW/GLFWWindow.h"
#include "Platform/Vulkan/VulkanInstance.h"
#include "Platform/Vulkan/VulkanDevice.h"
#include "Platform/Vulkan/VulkanPipeline.h"

#include "Graphics/Buffer/VertexBuffer.h"
#include "Graphics/Buffer/IndexBuffer.h"
#include "Platform/Vulkan/VulkanDescriptorSet.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Phoenix
{
  class VulkanRenderer : public Renderer
  {
  public:
    static constexpr int width = 1280;
    static constexpr int height = 720;
    const int max_frames_in_flight = 2;

    VulkanRenderer(GLFWWindow* window);
    ~VulkanRenderer();

    VulkanRenderer(const VulkanRenderer&) = delete;
    VulkanRenderer& operator=(const VulkanRenderer&) = delete;
    VulkanRenderer(VulkanRenderer&&) = delete;
    VulkanRenderer& operator=(VulkanRenderer&&) = delete;

    virtual void BeginRender() override;
    virtual void Render() override;
    virtual void EndRender() override;

    static VkCommandBuffer GetTempCommandBuffer();
    static void FlushTempCommandBuffer(VkCommandBuffer commandBuffer);

    [[nodiscard]] const VkInstance& GetVkInstance() { return m_Instance.GetInstance(); }
    [[nodiscard]] const VkPhysicalDevice& GetPhysicalDevice()
    {
      return m_Device.GetPhysicalDevice();
    }
    [[nodiscard]] const VkDevice& GetVkDevice() { return m_Device.GetDevice(); }
    [[nodiscard]] const VkQueue& GetGraphicsQueue() { return m_Device.GetGraphicsQueue(); }
    [[nodiscard]] QueueFamilyIndices GetGraphicsQueueFamily()
    {
      return m_Device.FindQueueFamilies(m_Device.GetPhysicalDevice());
    }
    [[nodiscard]] uint32_t GetImageCount() const { return m_ImageAvailableSemaphores.size(); }
    [[nodiscard]] const VkRenderPass& GetRenderPass() { return m_Device.GetRenderPass(); }
    [[nodiscard]] VkCommandBuffer& GetCurrentCommandBuffer()
    {
      return m_CommandBuffers[m_CurrentFrame];
    }
    [[nodiscard]] const VkPipeline& GetVkPipeline() { return m_SimplePipeline->GetVkPipeline(); }
    [[nodiscard]] const VkDescriptorPool& GetDescriptorPool() { return m_DescriptorPool; }
    [[nodiscard]] DescriptorSet* GetDescriptorSet() { return m_DescriptorSet; }
    [[nodiscard]] const VkCommandPool& GetCommandPool() { return m_Device.GetCommandPool(); }

  private:
    GLFWWindow* m_Window;
    VulkanInstance m_Instance{}; // TODO temp, might move to window class if this is per window
    VulkanDevice m_Device;
    VkPipelineLayout m_PipelineLayout{};
    std::unique_ptr<VulkanPipeline> m_SimplePipeline;

    uint32_t m_CurrentFrame = 0;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;

    VkDescriptorPool m_DescriptorPool;

    uint32_t imageIndex;

    const std::vector<Vertex> vertices = {{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                          {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                          {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
                                          {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

    const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};

    std::vector<UniformBuffer*> uniformBuffers;
    DescriptorSet* m_DescriptorSet;
    std::vector<VkDescriptorSet> m_DescriptorSets;

    VertexBuffer temp{m_Device, vertices};
    IndexBuffer temp2{m_Device, indices};

    void CreatePipelineLayout();
    void CreatePipeline();
    void CreateCommandBuffer();
    void CreateSyncObject();
    void CreateDescriptorPool();
    void CreateDescriptorSet();

  public:
    static void MakeDefault();

  protected:
    static Renderer* CreateFuncVulkan(Window* window);
  };
}
