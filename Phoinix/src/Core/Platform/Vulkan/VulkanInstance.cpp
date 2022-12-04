#include "VulkanInstance.h"

// #include "Application.h"
#include "VulkanValidation.h"

namespace Phoinix
{
   VulkanInstance::VulkanInstance()
   {
      ENGINE_TRACE("Creating Vulkan Instance");
      CreateInstance();
      SetupDebugMessenger();

      uint32_t extensionCount = 0;
      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
      std::vector<VkExtensionProperties> extensions(extensionCount);
      vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

      ENGINE_TRACE("Available extensions:");
      for (const auto& extension : extensions)
      {
         ENGINE_TRACE("\t{}", extension.extensionName);
      }
   }

   VulkanInstance::~VulkanInstance()
   {
      if (enable_validation_layers)
      {
         DestroyDebugUtilsMessengerExt(m_Instance, m_DebugMessenger, nullptr);
      }
      vkDestroyInstance(m_Instance, nullptr);
   }

   void VulkanInstance::CreateInstance()
   {
      if (enable_validation_layers && !CheckValidationLayerSupport())
      {
         ENGINE_ERR("Validation layers are enabled but none are available");
         std::exit(-2);
      }

      // WARNING: according to vulkan-tutorial.com, this current code will not work on MacOS
      // will require additional code when setting the extensions for the instance creation
      // see: https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance
      VkApplicationInfo appInfo{};
      appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
      appInfo.pApplicationName = "Vulkan App";
      appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
      appInfo.pEngineName = "Vulkan Engine";
      appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
      appInfo.apiVersion = VK_API_VERSION_1_0;

      VkInstanceCreateInfo createInfo{};
      createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
      createInfo.pApplicationInfo = &appInfo;

      uint32_t glfwExtensionCount = 0;
      const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

      std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

      if (enable_validation_layers)
      {
         extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
         createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
         createInfo.ppEnabledLayerNames = validation_layers.data();
      }
      else
      {
         createInfo.enabledLayerCount = 0;
      }

      createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
      createInfo.ppEnabledExtensionNames = extensions.data();

      VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
      if (enable_validation_layers)
      {
         createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
         createInfo.ppEnabledLayerNames = validation_layers.data();

         PopulateDebugMessengerCreateInfo(debugCreateInfo);
         createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
      }
      else
      {
         createInfo.enabledLayerCount = 0;
         createInfo.pNext = nullptr;
      }

      if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
      {
         ENGINE_ERR("Failed to create Vulkan Instance");
         std::exit(-3);
      }
   }

   void VulkanInstance::SetupDebugMessenger()
   {
      if (!enable_validation_layers)
      {
         return;
      }

      VkDebugUtilsMessengerCreateInfoEXT createInfo{};
      PopulateDebugMessengerCreateInfo(createInfo);

      if (CreateDebugUtilsMessengerExt(m_Instance, &createInfo, nullptr, &m_DebugMessenger) !=
          VK_SUCCESS)
      {
         ENGINE_ERR("Failed to setup validation layer debug messenger");
         std::exit(-3);
      }
   }

   VkResult
   VulkanInstance::CreateDebugUtilsMessengerExt(const VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger)
   {
      const auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
         instance, "vkCreateDebugUtilsMessengerEXT");
      if (func != nullptr)
      {
         return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
      }
      else
      {
         return VK_ERROR_EXTENSION_NOT_PRESENT;
      }
   }
   void VulkanInstance::DestroyDebugUtilsMessengerExt(const VkInstance instance,
                                                const VkDebugUtilsMessengerEXT pDebugMessenger,
                                                const VkAllocationCallbacks* pAllocator)
   {
      const auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
         instance, "vkDestroyDebugUtilsMessengerEXT");
      if (func != nullptr)
      {
         func(instance, pDebugMessenger, pAllocator);
      }
   }
   void VulkanInstance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
   {
      createInfo = {};
      createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
      createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
      createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
         VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
      createInfo.pfnUserCallback = DebugCallback;
   }
}
