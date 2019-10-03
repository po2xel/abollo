#ifndef __ABOLLO_GRAPHICS_VK_PHYSICAL_DEVICE_H__
#define __ABOLLO_GRAPHICS_VK_PHYSICAL_DEVICE_H__



#include <algorithm>
#include <string_view>
#include <vector>

#include "Utility.h"
#include "LogicalDevice.h"
#include "vk.h"



namespace abollo::vk
{


class PhysicalDevice final
{
private:
    VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};

public:
    PhysicalDevice() = default;

    explicit PhysicalDevice(const VkPhysicalDevice aPhysicalDevice) noexcept : mPhysicalDevice{aPhysicalDevice}
    {
    }

    constexpr operator const VkPhysicalDevice() const
    {
        return mPhysicalDevice;
    }

    [[nodiscard]] LogicalDevice CreateLogicalDevice(const VkDeviceCreateInfo& aDeviceCreateInfo) const
    {
        VkDevice lLogicalDevice{VK_NULL_HANDLE};

        ThrowIfFailed(vkCreateDevice(mPhysicalDevice, &aDeviceCreateInfo, nullptr, &lLogicalDevice));

        return lLogicalDevice;
    }

    [[nodiscard]] std::vector<VkLayerProperties> GetLayers() const
    {
        uint32_t lLayerCount{0};
        ThrowIfFailed(vkEnumerateDeviceLayerProperties(mPhysicalDevice, &lLayerCount, nullptr));

        std::vector<VkLayerProperties> lLayers{lLayerCount};
        ThrowIfFailed(vkEnumerateDeviceLayerProperties(mPhysicalDevice, &lLayerCount, lLayers.data()));

        return lLayers;
    }

    [[nodiscard]] std::vector<VkExtensionProperties> GetExtensions(const std::string_view aLayerName) const
    {
        uint32_t lExtensionCount{0};
        ThrowIfFailed(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, aLayerName.data(), &lExtensionCount, nullptr));

        std::vector<VkExtensionProperties> lExtensions{lExtensionCount};
        ThrowIfFailed(vkEnumerateDeviceExtensionProperties(mPhysicalDevice, aLayerName.data(), &lExtensionCount, lExtensions.data()));

        return {lExtensions.cbegin(), lExtensions.cend()};
    }

    [[nodiscard]] VkPhysicalDeviceProperties GetProperties() const
    {
        VkPhysicalDeviceProperties lProperties{};
        vkGetPhysicalDeviceProperties(mPhysicalDevice, &lProperties);

        return lProperties;
    }

    [[nodiscard]] VkPhysicalDeviceProperties GetProperties(void* apNext) const
    {
        VkPhysicalDeviceProperties2 lProperties2{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2, .pNext = apNext};
        vkGetPhysicalDeviceProperties2(mPhysicalDevice, &lProperties2);

        return lProperties2.properties;
    }

    [[nodiscard]] VkPhysicalDeviceFeatures GetFeatures() const
    {
        VkPhysicalDeviceFeatures lFeatures{};
        vkGetPhysicalDeviceFeatures(mPhysicalDevice, &lFeatures);

        return lFeatures;
    }

    [[nodiscard]] VkPhysicalDeviceFeatures GetFeatures(void* apNext) const
    {
        VkPhysicalDeviceFeatures2 lFeatures2{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = apNext};
        vkGetPhysicalDeviceFeatures2(mPhysicalDevice, &lFeatures2);

        return lFeatures2.features;
    }

    [[nodiscard]] VkFormatProperties GetFormatProperties(const VkFormat aFormat) const
    {
        VkFormatProperties lFormatProperties{};
        vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, aFormat, &lFormatProperties);

        return lFormatProperties;
    }

    [[nodiscard]] VkFormatProperties GetFormatProperties(const VkFormat aFormat, void* apNext) const
    {
        VkFormatProperties2 lFormatProperties2{.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2, .pNext = apNext};
        vkGetPhysicalDeviceFormatProperties2(mPhysicalDevice, aFormat, &lFormatProperties2);

        return lFormatProperties2.formatProperties;
    }

    [[nodiscard]] VkPhysicalDeviceMemoryProperties GetMemoryProperties() const
    {
        VkPhysicalDeviceMemoryProperties lMemoryProperties{};
        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &lMemoryProperties);

        return lMemoryProperties;
    }

    [[nodiscard]] VkPhysicalDeviceMemoryProperties GetMemoryProperties(void* apNext) const
    {
        VkPhysicalDeviceMemoryProperties2 lMemoryProperties2{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2, .pNext = apNext};
        vkGetPhysicalDeviceMemoryProperties2(mPhysicalDevice, &lMemoryProperties2);

        return lMemoryProperties2.memoryProperties;
    }

    [[nodiscard]] std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties() const
    {
        uint32_t lQueueFamilyPropertyCount{0};
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &lQueueFamilyPropertyCount, nullptr);

        std::vector<VkQueueFamilyProperties> lQueueFamilyProperties{lQueueFamilyPropertyCount};
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &lQueueFamilyPropertyCount, lQueueFamilyProperties.data());

        return lQueueFamilyProperties;
    }

    [[nodiscard]] std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties(void* apNext) const
    {
        uint32_t lQueueFamilyPropertyCount{0};
        vkGetPhysicalDeviceQueueFamilyProperties2(mPhysicalDevice, &lQueueFamilyPropertyCount, nullptr);

        std::vector<VkQueueFamilyProperties2> lQueueFamilyProperties2{lQueueFamilyPropertyCount, {.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2, .pNext = apNext}};
        vkGetPhysicalDeviceQueueFamilyProperties2(mPhysicalDevice, &lQueueFamilyPropertyCount, lQueueFamilyProperties2.data());

        std::vector<VkQueueFamilyProperties> lQueueFamilyProperties{lQueueFamilyPropertyCount};
        std::transform(lQueueFamilyProperties2.cbegin(), lQueueFamilyProperties2.cend(), lQueueFamilyProperties.begin(),
                       [](auto&& aProperty2) { return aProperty2.queueFamilyProperties; });

        return lQueueFamilyProperties;
    }

    [[nodiscard]] VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(const VkSurfaceKHR aSurface) const
    {
        VkSurfaceCapabilitiesKHR lSurfaceCapabilities{};
        ThrowIfFailed(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, aSurface, &lSurfaceCapabilities));

        return lSurfaceCapabilities;
    }

    [[nodiscard]] std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(const VkSurfaceKHR aSurface) const
    {
        uint32_t lSurfaceFormatCount{0};
        ThrowIfFailed(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, aSurface, &lSurfaceFormatCount, nullptr));

        std::vector<VkSurfaceFormatKHR> lSurfaceFormats{lSurfaceFormatCount};
        ThrowIfFailed(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, aSurface, &lSurfaceFormatCount, lSurfaceFormats.data()));

        return lSurfaceFormats;
    }

    [[nodiscard]] bool IsPresentationSupported(const uint32_t aQueueFamilyIndex) const
    {
#ifdef WIN32
        return VK_TRUE == vkGetPhysicalDeviceWin32PresentationSupportKHR(mPhysicalDevice, aQueueFamilyIndex);
#else
        static_assert(false, "Presentation is not supported.");
#endif
    }

    [[nodiscard]] bool IsPresentationSupported(const uint32_t aQueueFamilyIndex, const VkSurfaceKHR aSurface) const
    {
        VkBool32 lIsSupported{VK_FALSE};
        ThrowIfFailed(vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, aQueueFamilyIndex, aSurface, &lIsSupported));

        return VK_TRUE == lIsSupported;
    }

    [[nodiscard]] std::vector<VkPresentModeKHR> GetPresentModes(const VkSurfaceKHR aSurface) const
    {
        uint32_t lPresentModeCount{0};
        ThrowIfFailed(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, aSurface, &lPresentModeCount, nullptr));

        std::vector<VkPresentModeKHR> lPresentModes{lPresentModeCount};
        ThrowIfFailed(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, aSurface, &lPresentModeCount, lPresentModes.data()));

        return lPresentModes;
    }
};



}    // namespace abollo::vk



#endif    // __ABOLLO_GRAPHICS_VK_PHYSICAL_DEVICE_H__
