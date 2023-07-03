#include "UniformBuffer.h"

namespace Phoenix
{
  UniformBuffer* (*UniformBuffer::CreateDataFunc)(const UniformBufferObject&) = nullptr;
  UniformBuffer* (*UniformBuffer::CreateFunc)() = nullptr;

  UniformBuffer* UniformBuffer::Create()
  {
    PHOENIX_ASSERT(CreateFunc, "No UniformBuffer create function exists");
    return CreateFunc();
  }

  UniformBuffer* UniformBuffer::Create(const UniformBufferObject& data)
  {
    PHOENIX_ASSERT(CreateDataFunc, "No UniformBuffer create function exists");
    return CreateDataFunc(data);
  }
}
