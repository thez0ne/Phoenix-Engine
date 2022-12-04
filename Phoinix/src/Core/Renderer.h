#pragma once

#include "pch.h"

#include "Core/Core.h"
#include "Core/Events/Event.h"

namespace Phoinix
{
   class Renderer
   {
    public:
      //   using EventCallback = std::function<void(Event&)>;

      virtual ~Renderer() = default;

      virtual void DrawFrame() = 0;

      //   virtual unsigned int GetWidth() const = 0;
      //   virtual unsigned int GetHeight() const = 0;

      //   virtual void SetEventCallback(const EventCallback& _callback) = 0;

      //   virtual void SetVSync(bool enabled) = 0;
      //   virtual bool GetVSync() const = 0;

      //   virtual void* GetWindow() const = 0;

      static Renderer* Create();
   };
}
