#include "Renderer.h"

namespace Phoinix
{
   Renderer* (*Renderer::CreateFunc)(Window*) = nullptr;

   std::vector<std::function<void()>> Renderer::s_CleanupQueue;
   std::vector<std::function<void()>> Renderer::s_InstantCleanupQueue;

   // Renderer* Renderer::s_Instance = nullptr;

   Renderer* Renderer::Create(Window* window)
   {
      PHOINIX_ASSERT(CreateFunc, "No Renderer create function exists");
      return CreateFunc(window);
   }
}
