#pragma once

#include "pch.h"

#include "Core/Core.h"
#include "Events/Event.h"
#include "Window.h"

namespace Phoinix
{
   class Renderer
   {
    public:
      virtual ~Renderer() = default;

      virtual void DrawFrame() = 0;
      virtual void BeginRender() = 0;
      virtual void Render() = 0;
      virtual void EndRender() = 0;

      static Renderer* Create(Window* window);
   };
}
