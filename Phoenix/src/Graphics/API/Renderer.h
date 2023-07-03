#pragma once

#include "pch.h"

#include "Core/Core.h"
#include "Events/Event.h"
#include "Core/Window.h"

namespace Phoenix
{
  enum class RenderType
  {
    FILL,
    WIREFRAME
  };

  class Renderer
  {
  public:
    Renderer() = default;
    virtual ~Renderer() = default;

    virtual void BeginRender() = 0;
    virtual void Render() = 0;
    virtual void EndRender() = 0;

    static void RegisterCleanup(std::function<void()>&& func) { s_CleanupQueue.emplace_back(func); }

    static void RegisterInstantCleanup(std::function<void()>&& func)
    {
      s_InstantCleanupQueue.emplace_back(func);
    }

    static Renderer* Create(Window* window);

  protected:
    static Renderer* (*CreateFunc)(Window* window);

    static std::vector<std::function<void()>> s_CleanupQueue;
    static std::vector<std::function<void()>> s_InstantCleanupQueue;

    // uint32_t m_CurrentFrame = 0;
  };
}
