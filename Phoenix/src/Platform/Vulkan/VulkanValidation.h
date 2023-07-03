#pragma once
#include "pch.h"

#include "vulkan/vulkan.h"

// TODO might wanna just move this to the vk instance file

namespace Phoenix
{
  const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
#ifdef NDEBUG
  constexpr bool enable_validation_layers = false;
#else
  constexpr bool enable_validation_layers = true;
#endif

  bool CheckValidationLayerSupport();
  VKAPI_ATTR VkBool32 VKAPI_CALL
  DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData);
}
