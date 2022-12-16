#include "Image.h"

namespace Phoinix
{
   Image* (*Image::CreateFunc)(uint32_t width,
                               uint32_t height,
                               Format format,
                               void* data) = nullptr;

   Image* Image::Create(uint32_t width, uint32_t height, Format format, void* data)
   {
      PHOINIX_ASSERT(CreateFunc, "No image creation function exists");
      return CreateFunc(width, height, format, data);
   }
}
