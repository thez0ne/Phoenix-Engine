#include "Renderer.h"

namespace Phoenix
{
  Renderer* (*Renderer::CreateFunc)(Window*) = nullptr;

  std::vector<std::function<void()>> Renderer::s_CleanupQueue;
  std::vector<std::function<void()>> Renderer::s_InstantCleanupQueue;

  // Renderer* Renderer::s_Instance = nullptr;

  Renderer* Renderer::Create(Window* window)
  {
    PHOENIX_ASSERT(CreateFunc, "No Renderer create function exists");
    return CreateFunc(window);
  }
}
