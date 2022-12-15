#pragma once
#include "pch.h"

#include "Core/Core.h"
#include "Graphics/Buffer/Buffer.h"


namespace Phoinix
{
   class UniformBuffer
   {
    public:
      virtual ~UniformBuffer() = default;

      virtual void Update(uint32_t) = 0;

      static UniformBuffer* Create();
      static UniformBuffer* Create(const UniformBufferObject& data);

    protected:
      static UniformBuffer* (*CreateFunc)();
      static UniformBuffer* (*CreateDataFunc)(const UniformBufferObject& data);
   };
}