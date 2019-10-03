#ifndef __ABOLLO_GRAPHICS_VK_LOGICAL_DEVICE_H__
#define __ABOLLO_GRAPHICS_VK_LOGICAL_DEVICE_H__



#include <algorithm>

#include "Queue.h"
#include "Utility/NonCopyable.h"
#include "vk.h"



namespace abollo::vk
{



class LogicalDevice final : private internal::NonCopyable
{
private:
    VkDevice mDevice{VK_NULL_HANDLE};

public:
    LogicalDevice() noexcept = default;

    LogicalDevice(const VkDevice aDevice) noexcept : mDevice{aDevice}
    {
    }

    LogicalDevice(LogicalDevice&& aLogicalDevice) noexcept : mDevice{aLogicalDevice.mDevice}
    {
        aLogicalDevice.mDevice = VK_NULL_HANDLE;
    }

    ~LogicalDevice() noexcept
    {
        vkDestroyDevice(mDevice, nullptr);
    }

    void operator=(LogicalDevice&& aLogicalDevice) noexcept
    {
        if (mDevice != VK_NULL_HANDLE)
            vkDestroyDevice(mDevice, nullptr);

        mDevice = VK_NULL_HANDLE;

        std::swap(mDevice, aLogicalDevice.mDevice);
    }

    constexpr operator const VkDevice() const
    {
        return mDevice;
    }

    VkSwapchainKHR CreateSwapchain(const VkSwapchainCreateInfoKHR& aSwapchainCreateInfo) const
    {
        VkSwapchainKHR lSwapchain{VK_NULL_HANDLE};

        ThrowIfFailed(vkCreateSwapchainKHR(mDevice, &aSwapchainCreateInfo, nullptr, &lSwapchain));

        return lSwapchain;
    }

    [[nodiscard]] Queue GetQueue(const uint32_t aQueueFamilyIndex, const uint32_t aQueueIndex = 0) const
    {
        VkQueue lQueue{VK_NULL_HANDLE};

        vkGetDeviceQueue(mDevice, aQueueFamilyIndex, aQueueIndex, &lQueue);

        return lQueue;
    }

    [[nodiscard]] std::vector<VkImage> GetSwapchainImages(const VkSwapchainKHR aSwapchain) const
    {
        uint32_t lSwapchainImageCount{0};
        ThrowIfFailed(vkGetSwapchainImagesKHR(mDevice, aSwapchain, &lSwapchainImageCount, nullptr));

        std::vector<VkImage> lSwapchainImages{lSwapchainImageCount};
        ThrowIfFailed(vkGetSwapchainImagesKHR(mDevice, aSwapchain, &lSwapchainImageCount, lSwapchainImages.data()));

        return lSwapchainImages;
    }

    [[nodiscard]] VkSemaphore CreateSemaphore(const VkSemaphoreCreateInfo& aSemaphoreCreateInfo) const
    {
        VkSemaphore lSemaphore{};

        ThrowIfFailed(vkCreateSemaphore(mDevice, &aSemaphoreCreateInfo, nullptr, &lSemaphore));

        return lSemaphore;
    }

    void DestroySemaphore(const VkSemaphore& aSemaphore) const
    {
        vkDestroySemaphore(mDevice, aSemaphore, nullptr);
    }

    uint32_t AcquireNextImage(const VkSwapchainKHR aSwapchain, const VkSemaphore aSemaphore = VK_NULL_HANDLE, const VkFence aFence = VK_NULL_HANDLE, uint64_t aTimeOut = UINT64_MAX) const
    {
        uint32_t lImageIndex{UINT32_MAX};
        ThrowIfFailed(vkAcquireNextImageKHR(mDevice, aSwapchain, aTimeOut, aSemaphore, aFence, &lImageIndex));

        return lImageIndex;
    }

    void Wait() const
    {
        ThrowIfFailed(vkDeviceWaitIdle(mDevice));
    }
};



}    // namespace abollo::vk



#endif    // __ABOLLO_GRAPHICS_VK_LOGICAL_DEVICE_H__
