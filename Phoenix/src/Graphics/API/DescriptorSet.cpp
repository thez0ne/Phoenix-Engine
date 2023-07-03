#include "DescriptorSet.h"

namespace Phoenix
{
  DescriptorSet* (*DescriptorSet::CreateFunc)() = nullptr;

  DescriptorSet* DescriptorSet::Create()
  {
    PHOENIX_ASSERT(CreateFunc, "No Descriptor Set create function exists");
    return CreateFunc();
  }
}
