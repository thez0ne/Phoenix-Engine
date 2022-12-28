#include "DescriptorSet.h"

namespace Phoinix
{
  DescriptorSet* (*DescriptorSet::CreateFunc)() = nullptr;

  DescriptorSet* DescriptorSet::Create()
  {
    PHOINIX_ASSERT(CreateFunc, "No Descriptor Set create function exists");
    return CreateFunc();
  }
}
