#pragma once

#include <imgui.h>

#include "Core/Layer.h"

// TODO TEMP
#include <vulkan/vulkan.h>

namespace Phoinix
{
  class ImGuiLayer : public Layer
  {
  public:
    ImGuiLayer();
    ~ImGuiLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent(Event& e) override;

    virtual void OnImGUIUpdate() override;
    void ImGuiInitFrame();
    void ImGuiRenderFrame();

    inline ImGuiContext* GetImGuiCtx() { return ImGui::GetCurrentContext(); }

    void ShouldShowDemoWindow(bool status) { m_ShowDemo = status; }

  private:
    float dt = 0;
    VkDescriptorPool m_DescriptorPool;
    // GLFWwindow* win;

    bool m_ShowDemo;
  };
}
