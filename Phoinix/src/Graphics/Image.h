#pragma once
#include "pch.h"
#include "glm/vec4.hpp"

namespace Phoinix
{
   enum class Format
   {
      NONE = 0,
      RGBA,
      RGBA32F
   };

   static uint32_t BytesPerPixel(Format format)
   {
      switch (format)
      {
         case Format::NONE:
            PHOINIX_ASSERT(false, "Invalid image format given");
         case Format::RGBA:
            return 4;
         case Format::RGBA32F:
            return 16;
         default:
            return 0;
      }
   }

   class Image
   {
    public:
      Image(uint32_t width, uint32_t height, Format format, void* data = nullptr) :
         m_Width(width), m_Height(height), m_Format(format)
      {}
      virtual ~Image() = default;

      virtual void SetData(const void* data) = 0;
      virtual void ImGuiBind() = 0;
      virtual void* GetDescriptorSet() = 0;
      virtual void Resize(uint32_t, uint32_t) = 0;

      [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
      [[nodiscard]] uint32_t GetHeight() const { return m_Height; }

      static uint32_t VecToRgba(const glm::vec4& color)
      {
         const auto r = static_cast<uint8_t>(color.r * 255.0f);
         const auto g = static_cast<uint8_t>(color.g * 255.0f);
         const auto b = static_cast<uint8_t>(color.b * 255.0f);
         const auto a = static_cast<uint8_t>(color.a * 255.0f);

         return (a << 24) | (b << 16) | (g << 8) | r;
      }

      static Image* Create(uint32_t width, uint32_t height, Format format, void* data = nullptr);

    protected:
      static Image* (*CreateFunc)(uint32_t width, uint32_t height, Format format, void* data);

      uint32_t m_Width, m_Height;
      Format m_Format;
   };
}
