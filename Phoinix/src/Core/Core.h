#pragma once

#define NONCOPYABLE(TYPE)      \
   TYPE(const TYPE&) = delete; \
   TYPE& operator=(const TYPE&) = delete;

#ifdef PHOINIX_WINDOWS
   // Assertion
   #define PHOINIX_ASSERT(X, ...) \
      if (!(X))                   \
      {                           \
         ENGINE_ERR(__VA_ARGS__); \
         __debugbreak();          \
      }
   // Vulkan Assertion
   #define VKASSERT(X, ...)       \
      if ((X) != VK_SUCCESS)      \
      {                           \
         ENGINE_ERR(__VA_ARGS__); \
         __debugbreak();          \
      }

#else
   // Assertion
   #define PHOINIX_ASSERT(X, ...) \
      if (!(X))                   \
      {                           \
         ENGINE_ERR(__VA_ARGS__); \
         exit(-1);                \
      }
   // Vulkan Assertion
   #define VKASSERT(X, ...)       \
      if ((X) != VK_SUCCESS)      \
      {                           \
         ENGINE_ERR(__VA_ARGS__); \
         exit(-1);                \
      }

#endif
