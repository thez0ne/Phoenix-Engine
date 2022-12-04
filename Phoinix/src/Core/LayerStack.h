#pragma once

#include "pch.h"
#include "Core/Core.h"

#include "Core/Layer.h"

namespace Phoinix
{
   typedef std::vector<Layer*> LayerVector;
   typedef std::vector<Layer*>::iterator LayerIterator;

   class LayerStack
   {
    public:
      LayerStack();
      ~LayerStack();

      void PushLayer(Layer* layer);
      void PushOverlay(Layer* layer);

      LayerIterator begin() { return stack.begin(); }
      LayerIterator end() { return stack.end(); }

    private:
      LayerVector stack;
      LayerIterator currentLayer;
      unsigned int currentLayerIndex = 0;
   };
}