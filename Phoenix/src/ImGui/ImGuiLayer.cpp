#include "pch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include "Core/Application.h"
// #include "Platform/OpenGL/OpenGLWindow.h"
// #include "Platform/Vulkan/GLFWWindow.h"
#include "Graphics/Renderer/VulkanRenderer.h"

namespace Phoenix
{
  ImGuiLayer::ImGuiLayer() : Layer("ImGui Layer") {}

  ImGuiLayer::~ImGuiLayer()
  {
    OnDetach();
  }

  void ImGuiLayer::OnAttach()
  {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

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

      VKASSERT(
        vkCreateDescriptorPool(VulkanDevice::Device(), &pool_info, nullptr, &m_DescriptorPool),
        "Failed to create descriptor pool");
    }

    auto app = Application::GetInstance();
    Window* win = &app->GetWindow(); // TODO store reference
    auto renderer = (VulkanRenderer*)&app->GetRenderer();
    ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)win->GetWindow(), true);

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = renderer->GetVkInstance();
    initInfo.PhysicalDevice = renderer->GetPhysicalDevice();
    initInfo.Device = VulkanDevice::Device();
    initInfo.QueueFamily = renderer->GetGraphicsQueueFamily().graphicsFamily.value();
    initInfo.Queue = VulkanDevice::GraphicsQueue();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.DescriptorPool = m_DescriptorPool;
    initInfo.Subpass = 0;       // TODO: check if it is 0 with our render settings
    initInfo.MinImageCount = 2; // TODO: check if right value
    initInfo.ImageCount = renderer->GetImageCount();
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = [](VkResult err) {
      VKASSERT(err, "[ImGUI] error with VkResult: {}", err);
    };

    ImGui_ImplVulkan_Init(&initInfo, renderer->GetRenderPass());

    // Upload Fonts
    {
      // Use any command queue
      VkCommandPool command_pool =
        renderer->GetCommandPool(); // TODO create own command pool for this action

      // create command buffer for the ImGUI fonts
      VkCommandBuffer commandBuffer;
      {
        // TODO can probably consolidate this into a function
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = command_pool;
        allocInfo.commandBufferCount = 1;

        VKASSERT(vkAllocateCommandBuffers(VulkanDevice::Device(), &allocInfo, &commandBuffer),
                 "Failed to allocate command buffer");

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VKASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin command buffer");
      }

      ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

      // end and delete command buffer for the ImGUI fonts
      {
        // TODO can probably consolidate this into a function
        VKASSERT(vkEndCommandBuffer(commandBuffer), "Failed to end command buffer");

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VKASSERT(vkQueueSubmit(VulkanDevice::GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE),
                 "Failed to submit to queue");
        VKASSERT(vkQueueWaitIdle(VulkanDevice::GraphicsQueue()),
                 "Failed to wait for queue to idle");
        VKASSERT(vkDeviceWaitIdle(VulkanDevice::Device()), "Failed to wait for device to idle");

        vkFreeCommandBuffers(VulkanDevice::Device(), command_pool, 1, &commandBuffer);
      }
      ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
  }

  void ImGuiLayer::OnDetach()
  {
    vkDeviceWaitIdle(VulkanDevice::Device());

    vkDestroyDescriptorPool(VulkanDevice::Device(), m_DescriptorPool, nullptr);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void ImGuiLayer::OnUpdate() {}

  void ImGuiLayer::OnEvent(Event& e) {}

  void ImGuiLayer::OnImGUIUpdate()
  {
    if (m_ShowDemo)
      ImGui::ShowDemoWindow();
  }

  void ImGuiLayer::ImGuiInitFrame()
  {
    // TODO add handling of swapchain recreation
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
  }

  void ImGuiLayer::ImGuiRenderFrame()
  {
    ImGuiIO& io = ImGui::GetIO();
    Application* app = Application::GetInstance();
    io.DisplaySize = ImVec2(app->GetWindow().GetWidth(), app->GetWindow().GetHeight());

    ImGui::Render();
    auto renderer = (VulkanRenderer*)&app->GetRenderer();
    vkDeviceWaitIdle(VulkanDevice::Device());
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), renderer->GetCurrentCommandBuffer());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
  }
}
