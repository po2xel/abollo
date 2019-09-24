#ifndef __ABOLLO_GRAPHICS_VULKAN_CONTEXT_H__
#define __ABOLLO_GRAPHICS_VULKAN_CONTEXT_H__


#include <string_view>


#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#undef VK_NULL_HANDLE
#define VK_NULL_HANDLE nullptr

#include "Utility/NonCopyable.h"



namespace abollo
{



class Window;



class VulkanContext final : private internal::NonCopyable
{
private:
    struct DeviceQueue
    {
        VkQueue handle{VK_NULL_HANDLE};
        uint32_t familyIndex{UINT32_MAX};
    };

    VkInstance mInstance{VK_NULL_HANDLE};
    VkSurfaceKHR mSurface{VK_NULL_HANDLE};

    VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};
    DeviceQueue mGraphicsQueue;
    DeviceQueue mPresentQueue;
    VkDevice mDevice{VK_NULL_HANDLE};

    [[nodiscard]] bool IsSurfaceSupported(const uint32_t aQueueFamilyIndex) const;

public:
    VulkanContext(const Window& aWindow, const std::string_view aAppName, const uint32_t aAppVersion, const std::string_view aEngineName, const uint32_t aEngineVersion);
    ~VulkanContext();
};



}    // namespace abollo



#endif    // __ABOLLO_GRAPHICS_VULKAN_CONTEXT_H__
