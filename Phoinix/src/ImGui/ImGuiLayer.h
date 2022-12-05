#pragma once

#include <imgui.h>
#include "Core/Layer.h"

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

    private:
      float dt = 0;
      // GLFWwindow* win;
   };
}
