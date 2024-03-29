#pragma once

#include "pch.h"
#include "Core/Core.h"
#include "Events/Event.h"

namespace Phoenix
{
  class Layer
  {
  public:
    Layer(const std::string& name = "LayerName") : m_Name(name) {}
    virtual ~Layer() {}

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate() {}
    virtual void OnImGUIUpdate() {}
    virtual void OnEvent(Event& e) {}

    inline const std::string GetName() const { return m_Name; }

  private:
    std::string m_Name;
  };
}
