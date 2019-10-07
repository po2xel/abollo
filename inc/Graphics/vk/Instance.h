#ifndef __ABOLLO_GRAPHICS_VK_INSTANCE_H__
#define __ABOLLO_GRAPHICS_VK_INSTANCE_H__



#include <array>
#include <string_view>
#include <vector>

#include "PhysicalDevice.h"
#include "Utility.h"
#include "Utility/NonCopyable.h"
#include "vk.h"



namespace abollo::vk
{



constexpr auto DEFAULT_VULKAN_VERSION  = VK_API_VERSION_1_1;
constexpr auto DEFAULT_VALIDATION_NAME = "VK_LAYER_LUNARG_standard_validation";



class Instance final : private internal::NonCopyable
{
private:
    VkInstance mInstance{VK_NULL_HANDLE};

public:
    Instance(const std::string_view aAppName, const uint32_t aAppVersion, const std::string_view aEngineName, const uint32_t aEngineVersion)
    {
        const VkApplicationInfo lApplicationInfo{.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                                 .pNext              = nullptr,
                                                 .pApplicationName   = aAppName.data(),
                                                 .applicationVersion = aAppVersion,
                                                 .pEngineName        = aEngineName.data(),
                                                 .engineVersion      = aEngineVersion,
                                                 .apiVersion         = DEFAULT_VULKAN_VERSION};

#ifdef NDEBUG
        constexpr std::array<const char*, 0> lInstanceLayers{};
#else
        constexpr std::array<const char*, 1> lInstanceLayers{DEFAULT_VALIDATION_NAME};
#endif

        constexpr std::array<const char*, 3> lInstanceExtensions{VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME};

        constexpr VkInstanceCreateInfo lInstanceCreateInfo{.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                                           .pNext                   = nullptr,
                                                           .flags                   = 0,
                                                           .pApplicationInfo        = &lApplicationInfo,
                                                           .enabledLayerCount       = static_cast<uint32_t>(lInstanceLayers.size()),
                                                           .ppEnabledLayerNames     = lInstanceLayers.data(),
                                                           .enabledExtensionCount   = static_cast<uint32_t>(lInstanceExtensions.size()),
                                                           .ppEnabledExtensionNames = lInstanceExtensions.data()};

        ThrowIfFailed(vkCreateInstance(&lInstanceCreateInfo, nullptr, &mInstance));
    }

    ~Instance() noexcept
    {
        vkDestroyInstance(mInstance, nullptr);
    }

    constexpr operator const VkInstance() const
    {
        return mInstance;
    }

    [[nodiscard]] std::vector<PhysicalDevice> GetPhysicalDevices() const
    {
        uint32_t lPhysicalDeviceCount{0};
        ThrowIfFailed(vkEnumeratePhysicalDevices(mInstance, &lPhysicalDeviceCount, nullptr));

        std::vector<VkPhysicalDevice> lPhysicalDevices{lPhysicalDeviceCount};
        ThrowIfFailed(vkEnumeratePhysicalDevices(mInstance, &lPhysicalDeviceCount, lPhysicalDevices.data()));

        return {lPhysicalDevices.cbegin(), lPhysicalDevices.cend()};
    }

    static std::vector<VkLayerProperties> GetLayers()
    {
        uint32_t lLayerCount{0};
        ThrowIfFailed(vkEnumerateInstanceLayerProperties(&lLayerCount, nullptr));

        std::vector<VkLayerProperties> lLayers{lLayerCount};
        ThrowIfFailed(vkEnumerateInstanceLayerProperties(&lLayerCount, lLayers.data()));

        return lLayers;
    }

    static std::vector<VkExtensionProperties> GetExtensions(const std::string_view aLayerName)
    {
        uint32_t lExtensionCount{0};
        ThrowIfFailed(vkEnumerateInstanceExtensionProperties(aLayerName.data(), &lExtensionCount, nullptr));

        std::vector<VkExtensionProperties> lExtensions{lExtensionCount};
        ThrowIfFailed(vkEnumerateInstanceExtensionProperties(aLayerName.data(), &lExtensionCount, lExtensions.data()));

        return lExtensions;
    }
};



}    // namespace abollo::vk



#endif    // __ABOLLO_GRAPHICS_VK_INSTANCE_H__
