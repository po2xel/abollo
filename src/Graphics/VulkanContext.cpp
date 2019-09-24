#include "Graphics/VulkanContext.h"

#include <array>
#include <cassert>
#include <vector>

#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>

#include "Window/Window.h"



namespace abollo
{



namespace vk
{



constexpr auto DEFAULT_VULKAN_VERSION  = VK_API_VERSION_1_1;
constexpr auto DEFAULT_VALIDATION_NAME = "VK_LAYER_LUNARG_standard_validation";



void ThrowIfFailed(VkResult aResult)
{
    if (aResult != VK_SUCCESS)
        throw;
}


std::vector<VkLayerProperties> GetLayers()
{
    uint32_t lLayerCount{0};
    ThrowIfFailed(vkEnumerateInstanceLayerProperties(&lLayerCount, nullptr));
    assert(lLayerCount != 0);

    std::vector<VkLayerProperties> lLayers{lLayerCount};
    ThrowIfFailed(vkEnumerateInstanceLayerProperties(&lLayerCount, lLayers.data()));

    return lLayers;
}


std::vector<VkExtensionProperties> GetExtensions(const std::string_view aLayerName)
{
    uint32_t lExtCount{0};
    ThrowIfFailed(vkEnumerateInstanceExtensionProperties(aLayerName.data(), &lExtCount, nullptr));
    assert(lExtCount != 0);

    std::vector<VkExtensionProperties> lExtensions{lExtCount};
    ThrowIfFailed(vkEnumerateInstanceExtensionProperties(aLayerName.data(), &lExtCount, lExtensions.data()));

    return lExtensions;
}


VkInstance CreateInstance(const std::string_view aAppName, const uint32_t aAppVersion, const std::string_view aEngineName, const uint32_t aEngineVersion)
{
    const VkApplicationInfo lApplicationInfo{.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                             .pNext              = nullptr,
                                             .pApplicationName   = aAppName.data(),
                                             .applicationVersion = aAppVersion,
                                             .pEngineName        = aEngineName.data(),
                                             .engineVersion      = aEngineVersion,
                                             .apiVersion         = DEFAULT_VULKAN_VERSION};

    constexpr std::array<const char*, 1> lInstanceLayers{DEFAULT_VALIDATION_NAME};
    constexpr std::array<const char*, 3> lInstanceExtensions{VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME};

    constexpr VkInstanceCreateInfo lInstanceCreateInfo{.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                                       .pNext                   = nullptr,
                                                       .flags                   = 0,
                                                       .pApplicationInfo        = &lApplicationInfo,
                                                       .enabledLayerCount       = static_cast<uint32_t>(lInstanceLayers.size()),
                                                       .ppEnabledLayerNames     = lInstanceLayers.data(),
                                                       .enabledExtensionCount   = static_cast<uint32_t>(lInstanceExtensions.size()),
                                                       .ppEnabledExtensionNames = lInstanceExtensions.data()};

    VkInstance lInstance{};
    ThrowIfFailed(vkCreateInstance(&lInstanceCreateInfo, nullptr, &lInstance));

    return lInstance;
}



}    // namespace vk



bool VulkanContext::IsSurfaceSupported(const uint32_t aQueueFamilyIndex) const
{
    VkBool32 lIsSupported{VK_FALSE};

    vk::ThrowIfFailed(vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, aQueueFamilyIndex, mSurface, &lIsSupported));

    return VK_TRUE == lIsSupported;
}


VulkanContext::VulkanContext(const Window& aWindow, const std::string_view aAppName, const uint32_t aAppVersion, const std::string_view aEngineName, const uint32_t aEngineVersion)
    : mInstance(vk::CreateInstance(aAppName, aAppVersion, aEngineName, aEngineVersion))
{
    if (SDL_TRUE != SDL_Vulkan_CreateSurface(aWindow.GetHandle(), mInstance, &mSurface))
        throw std::runtime_error("Failed to create vulkan surface");

    uint32_t lPhysicalDeviceCount{0};
    vk::ThrowIfFailed(vkEnumeratePhysicalDevices(mInstance, &lPhysicalDeviceCount, nullptr));
    assert(lPhysicalDeviceCount != 0);

    std::vector<VkPhysicalDevice> lPhysicalDevices{lPhysicalDeviceCount};
    vkEnumeratePhysicalDevices(mInstance, &lPhysicalDeviceCount, lPhysicalDevices.data());

    if (lPhysicalDeviceCount == 1)
        mPhysicalDevice = lPhysicalDevices.front();
    else
    {
        for (const auto& lPhysicalDevice : lPhysicalDevices)
        {
            VkPhysicalDeviceProperties lPhysicalDeviceProperties{};
            vkGetPhysicalDeviceProperties(lPhysicalDevice, &lPhysicalDeviceProperties);

            if (lPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                mPhysicalDevice = lPhysicalDevice;
                break;
            }
        }

        if (VK_NULL_HANDLE == mPhysicalDevice)
            mPhysicalDevice = lPhysicalDevices.front();
    }

    uint32_t lQueueFamilyPropertyCount{0};
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &lQueueFamilyPropertyCount, nullptr);
    assert(lQueueFamilyPropertyCount != 0);

    std::vector<VkQueueFamilyProperties> lQueueFamilyProperties{lQueueFamilyPropertyCount};
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &lQueueFamilyPropertyCount, lQueueFamilyProperties.data());

    if (lQueueFamilyPropertyCount == 1)
    {
        mGraphicsQueue.familyIndex = 0;
        mPresentQueue.familyIndex  = 0;
    }
    else
    {
        const auto lSize = lQueueFamilyProperties.size();

        for (uint32_t lIndex = 0; lIndex < lSize; ++lIndex)
        {
            if ((lQueueFamilyProperties.at(lIndex).queueFlags & VK_QUEUE_GRAPHICS_BIT) && IsSurfaceSupported(lIndex))
            {
                mGraphicsQueue.familyIndex = lIndex;
                mPresentQueue.familyIndex  = lIndex;
                break;
            }
        }
    }

    constexpr float lQueuePriorities[]                 = {0.f};
    const VkDeviceQueueCreateInfo lQueueCreateInfos[2] = {{.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                           .pNext            = nullptr,
                                                           .flags            = 0,
                                                           .queueFamilyIndex = mGraphicsQueue.familyIndex,
                                                           .queueCount       = 1,
                                                           .pQueuePriorities = lQueuePriorities},
                                                          {.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                           .pNext            = nullptr,
                                                           .flags            = 0,
                                                           .queueFamilyIndex = mPresentQueue.familyIndex,
                                                           .queueCount       = 1,
                                                           .pQueuePriorities = lQueuePriorities}};

    const uint32_t lQueueInfoCount = mGraphicsQueue.familyIndex != mPresentQueue.familyIndex ? 2 : 1;

    const VkDeviceCreateInfo lDeviceCreateInfo{.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                               .pNext                   = nullptr,
                                               .flags                   = 0,
                                               .queueCreateInfoCount    = lQueueInfoCount,
                                               .pQueueCreateInfos       = lQueueCreateInfos,
                                               .enabledLayerCount       = 0,
                                               .ppEnabledLayerNames     = nullptr,
                                               .enabledExtensionCount   = 0,
                                               .ppEnabledExtensionNames = nullptr,
                                               .pEnabledFeatures        = nullptr};

    vk::ThrowIfFailed(vkCreateDevice(mPhysicalDevice, &lDeviceCreateInfo, nullptr, &mDevice));
}


VulkanContext::~VulkanContext()
{
    vkDestroyDevice(mDevice, nullptr);
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    vkDestroyInstance(mInstance, nullptr);
}



}    // namespace abollo