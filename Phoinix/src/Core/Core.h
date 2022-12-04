#pragma once

#ifdef PHOINIX_WINDOWS
   // Assertion
   #define PHOINIX_ASSERT(X, ...) \
      if (!(X))                   \
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

#endif
