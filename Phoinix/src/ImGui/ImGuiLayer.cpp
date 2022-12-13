#include "pch.h"
#include "ImGuiLayer.h"

// #include "Platform/ImGui/imgui_impl_opengl3.h"
// #include "Platform/ImGui/imgui_impl_glfw.h"
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include "Core/Application.h"
// #include "Platform/OpenGL/OpenGLWindow.h"
#include "Platform/Vulkan/VulkanWindow.h"
#include "Renderer/VulkanRenderer.h"

namespace Phoinix
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

      auto app = Application::GetInstance();
      Window* win = &app->GetWindow(); // TODO store reference
      auto renderer = (VulkanRenderer*)&app->GetRenderer();
      ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)win->GetWindow(), true);

      ImGui_ImplVulkan_InitInfo initInfo{};
      initInfo.Instance = renderer->GetVkInstance();
      initInfo.PhysicalDevice = renderer->GetPhysicalDevice();
      initInfo.Device = renderer->GetVkDevice();
      initInfo.QueueFamily = renderer->GetGraphicsQueueFamily().graphicsFamily.value();
      initInfo.Queue = renderer->GetGraphicsQueue();
      initInfo.PipelineCache = VK_NULL_HANDLE;
      initInfo.DescriptorPool = renderer->GetDescriptorPool();
      initInfo.Subpass = 0;       // TODO: check if it is 0 with our render settings
      initInfo.MinImageCount = 2; // TODO: check if right value
      initInfo.ImageCount = renderer->GetImageCount();
      initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
      initInfo.Allocator = nullptr;
      initInfo.CheckVkResultFn = [](VkResult err) {
         if (err != VK_SUCCESS)
         {
            ENGINE_ERR("[ImGUI] error with VkResult: {}", err);
            std::exit(-3);
         }
      };

      ImGui_ImplVulkan_Init(&initInfo, renderer->GetRenderPass());

      // Upload Fonts
      {
         // Use any command queue
         VkCommandPool command_pool = renderer->GetCommandPool(); // TODO create own command pool for this action
         // VkCommandBuffer command_buffer = renderer->GetCurrentCommandBuffer();

         // create command buffer for the ImGUI fonts
         VkCommandBuffer commandBuffer;
         {
            // TODO can probably consolidate this into a function
            // TODO add validation?
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = command_pool;
            allocInfo.commandBufferCount = 1;

            vkAllocateCommandBuffers(renderer->GetVkDevice(), &allocInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);
         }

         ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

         // end and delete command buffer for the ImGUI fonts
         {
            // TODO can probably consolidate this into a function
            // TODO add validation?
            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(renderer->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(renderer->GetGraphicsQueue());
            vkDeviceWaitIdle(renderer->GetVkDevice());

            vkFreeCommandBuffers(renderer->GetVkDevice(), command_pool, 1, &commandBuffer);
         }
         ImGui_ImplVulkan_DestroyFontUploadObjects();
      }
   }

   void ImGuiLayer::OnDetach()
   {
      // TODO currently causes validation layer error
      // destroys VkPipeline that is in use by command buffer
      ImGui_ImplVulkan_Shutdown();
      ImGui_ImplGlfw_Shutdown();
      ImGui::DestroyContext();
   }

   void ImGuiLayer::OnUpdate() {}

   void ImGuiLayer::OnEvent(Event& e) {}

   void ImGuiLayer::OnImGUIUpdate()
   {
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
      vkDeviceWaitIdle(renderer->GetVkDevice());
      ImGui_ImplVulkan_RenderDrawData(
         ImGui::GetDrawData(), renderer->GetCurrentCommandBuffer());

      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
         GLFWwindow* backup_current_context = glfwGetCurrentContext();
         ImGui::UpdatePlatformWindows();
         ImGui::RenderPlatformWindowsDefault();
         glfwMakeContextCurrent(backup_current_context);
      }
   }
}
