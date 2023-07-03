#include "LayerStack.h"

namespace Phoenix
{
  LayerStack::LayerStack() {}

  LayerStack::~LayerStack()
  {
    for (Layer* layer : stack)
      delete layer;
  }

  void LayerStack::PushLayer(Layer* layer)
  {
    stack.emplace(stack.begin() + currentLayerIndex, layer);
    currentLayerIndex++;
    layer->OnAttach();
  }

  void LayerStack::PushOverlay(Layer* layer)
  {
    stack.emplace_back(layer);
    layer->OnAttach();
  }

  void LayerStack::PopLayer(Layer* layer)
  {
    stack.erase(std::ranges::find(stack, layer));
    delete layer;
  }
}
