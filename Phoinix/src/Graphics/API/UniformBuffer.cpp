#include "UniformBuffer.h"

namespace Phoinix
{
   UniformBuffer* (*UniformBuffer::CreateDataFunc)(const UniformBufferObject&) = nullptr;
   UniformBuffer* (*UniformBuffer::CreateFunc)() = nullptr;

   UniformBuffer* UniformBuffer::Create()
   {
      PHOINIX_ASSERT(CreateFunc, "No UniformBuffer create function exists");
      return CreateFunc();
   }

   UniformBuffer* UniformBuffer::Create(const UniformBufferObject& data)
   {
      PHOINIX_ASSERT(CreateDataFunc, "No UniformBuffer create function exists");
      return CreateDataFunc(data);
   }
}
