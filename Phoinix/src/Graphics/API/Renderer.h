#pragma once

#include "pch.h"

#include "Core/Core.h"
#include "Events/Event.h"
#include "Core/Window.h"

namespace Phoinix
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

      static Renderer* (*CreateFunc)(Window* window);
   };
}
