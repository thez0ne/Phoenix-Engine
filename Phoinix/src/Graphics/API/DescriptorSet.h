#pragma once
#include "pch.h"

#include "Core/Core.h"

namespace Phoinix
{
  class DescriptorSet
  {
  public:
    DescriptorSet() = default;
    virtual ~DescriptorSet() = default;

    static DescriptorSet* Create();

  protected:
    static DescriptorSet* (*CreateFunc)();
  };
}
