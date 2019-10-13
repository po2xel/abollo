#include "Graphics/VulkanContext.h"

#include <array>
#include <cassert>
#include <vector>

#include <SDL2/SDL_vulkan.h>
#include <fmt/format.h>
#include <skia/include/core/SkGraphics.h>
#include <skia/include/gpu/GrBackendSemaphore.h>
#include <skia/include/gpu/vk/GrVkBackendContext.h>
#include <skia/include/gpu/vk/GrVkExtensions.h>

#include "Window/Window.h"



namespace abollo
{


VulkanContext::VulkanContext(const Window& aWindow, const std::string_view aAppName, const uint32_t aAppVersion, const std::string_view aEngineName, const uint32_t aEngineVersion)
    : mInstance{aAppName, aAppVersion, aEngineName, aEngineVersion}
{
    SkGraphics::Init();

    // Create vulkan surface
    if (SDL_TRUE != SDL_Vulkan_CreateSurface(aWindow.GetHandle(), mInstance, &mSurface))
        throw std::runtime_error("Failed to create vulkan surface.");

    // Pick a physical device
    const auto& lPhysicalDevices = mInstance.GetPhysicalDevices();

    if (lPhysicalDevices.size() == 1)
        mPhysicalDevice = lPhysicalDevices.front();
    else
    {
        for (const auto& lPhysicalDevice : lPhysicalDevices)
        {
            if (VkPhysicalDeviceProperties lPhysicalDeviceProperties{};
                vkGetPhysicalDeviceProperties(lPhysicalDevice, &lPhysicalDeviceProperties), VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == lPhysicalDeviceProperties.deviceType)
            {
                mPhysicalDevice = lPhysicalDevice;
                break;
            }
        }

        if (VK_NULL_HANDLE == mPhysicalDevice)
            mPhysicalDevice = lPhysicalDevices.front();
    }

    // Fetch graphics and presentation queue family indexes
    const auto& lQueueFamilyProperties = mPhysicalDevice.GetQueueFamilyProperties();
    const auto lSize                   = lQueueFamilyProperties.size();

    for (uint32_t lIndex = 0; lIndex < lSize; ++lIndex)
    {
        if ((lQueueFamilyProperties.at(lIndex).queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && mPhysicalDevice.IsPresentationSupported(lIndex, mSurface))
        {
            mGraphicsQueue.familyIndex = lIndex;
            mPresentQueue.familyIndex  = lIndex;
            break;
        }
    }

    assert(mGraphicsQueue.IsValid() && mPresentQueue.IsValid());

    // Create logical device
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

    const auto& lPhysicalDeviceFeatures = mPhysicalDevice.GetFeatures();

    const uint32_t lQueueInfoCount = mGraphicsQueue.familyIndex != mPresentQueue.familyIndex ? 2 : 1;
    constexpr std::array<const char*, 1> lDeviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    const VkDeviceCreateInfo lDeviceCreateInfo{.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                               .pNext                   = nullptr,
                                               .flags                   = 0,
                                               .queueCreateInfoCount    = lQueueInfoCount,
                                               .pQueueCreateInfos       = lQueueCreateInfos,
                                               .enabledLayerCount       = 0,
                                               .ppEnabledLayerNames     = nullptr,
                                               .enabledExtensionCount   = static_cast<uint32_t>(lDeviceExtensions.size()),
                                               .ppEnabledExtensionNames = lDeviceExtensions.data(),
                                               .pEnabledFeatures        = &lPhysicalDeviceFeatures};

    mLogicalDevice = mPhysicalDevice.CreateLogicalDevice(lDeviceCreateInfo);

    // Fetch graphics and presentation queue
    mGraphicsQueue.handle = mLogicalDevice.GetQueue(mGraphicsQueue.familyIndex);
    mPresentQueue.handle  = mLogicalDevice.GetQueue(mPresentQueue.familyIndex);

    const auto lGetProc = [](const char* apProcName, VkInstance aInstance, VkDevice aDevice) {
        if (aDevice != VK_NULL_HANDLE)
            return vkGetDeviceProcAddr(aDevice, apProcName);

        return vkGetInstanceProcAddr(aInstance, apProcName);
    };

#ifdef NDEBUG
    constexpr std::array<const char*, 2> lInstanceExtensions{VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
#else
    constexpr std::array<const char*, 3> lInstanceExtensions{VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME};
#endif

    GrVkExtensions lExtensions;
    lExtensions.init(lGetProc, mInstance, mPhysicalDevice, static_cast<uint32_t>(lInstanceExtensions.size()) , lInstanceExtensions.data(), 1, lDeviceExtensions.data());

    const GrVkBackendContext lBackendContext{.fInstance           = mInstance,
                                             .fPhysicalDevice     = mPhysicalDevice,
                                             .fDevice             = mLogicalDevice,
                                             .fQueue              = mGraphicsQueue.handle,
                                             .fGraphicsQueueIndex = mGraphicsQueue.familyIndex,
                                             .fMaxAPIVersion      = VK_VERSION_1_1,
                                             .fVkExtensions       = &lExtensions,
                                             .fDeviceFeatures     = &lPhysicalDeviceFeatures,
                                             .fDeviceFeatures2    = nullptr,
                                             .fGetProc            = lGetProc};

    mContext = GrContext::MakeVulkan(lBackendContext, mContextOptions);

    CreateSwapchain();
}


VulkanContext::~VulkanContext()
{
    mPresentQueue.handle.Wait();
    mLogicalDevice.Wait();

    DestroyBuffers();
    vkDestroySwapchainKHR(mLogicalDevice, mSwapchain, nullptr);

    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

    mContext.reset();

    // vkDestroyDevice(mDevice, nullptr);
    // vkDestroyInstance(mInstance, nullptr);
}


void VulkanContext::CreateSwapchain()
{
    const auto& lSurfaceCapabilities = mPhysicalDevice.GetSurfaceCapabilities(mSurface);

    const VkExtent2D lExtent{
        .width  = std::max(lSurfaceCapabilities.minImageExtent.width, std::min(lSurfaceCapabilities.maxImageExtent.width, lSurfaceCapabilities.currentExtent.width)),
        .height = std::max(lSurfaceCapabilities.minImageExtent.height, std::min(lSurfaceCapabilities.maxImageExtent.height, lSurfaceCapabilities.currentExtent.height))};

    const auto lImageCount                       = std::min(lSurfaceCapabilities.minImageCount + 2, lSurfaceCapabilities.maxImageCount);
    constexpr VkImageUsageFlags lImageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    const auto lCompositeAlpha =
        (lSurfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) != 0 ? VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    // const auto& lSurfaceFormats = mPhysicalDevice.GetSurfaceFormats(mSurface);

    constexpr auto lSurfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
    constexpr auto lColorSpace    = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    // const auto& lPresentModes   = mPhysicalDevice.GetPresentModes(mSurface);

    constexpr auto lPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    VkSwapchainCreateInfoKHR lSwapchainCreateInfo{.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                                  .pNext            = nullptr,
                                                  .flags            = 0,
                                                  .surface          = mSurface,
                                                  .minImageCount    = lImageCount,
                                                  .imageFormat      = lSurfaceFormat,
                                                  .imageColorSpace  = lColorSpace,
                                                  .imageExtent      = lExtent,
                                                  .imageArrayLayers = 1,
                                                  .imageUsage       = lImageUsageFlags,
                                                  .preTransform     = lSurfaceCapabilities.currentTransform,
                                                  .compositeAlpha   = lCompositeAlpha,
                                                  .presentMode      = lPresentMode,
                                                  .clipped          = VK_TRUE,
                                                  .oldSwapchain     = mSwapchain

    };

    const uint32_t lQueueFamilies[] = {mGraphicsQueue.familyIndex, mPresentQueue.familyIndex};
    if (mGraphicsQueue.familyIndex != mPresentQueue.familyIndex)
    {
        lSwapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        lSwapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(std::size(lQueueFamilies));
        lSwapchainCreateInfo.pQueueFamilyIndices   = lQueueFamilies;
    }
    else
    {
        lSwapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        lSwapchainCreateInfo.queueFamilyIndexCount = 0;
        lSwapchainCreateInfo.pQueueFamilyIndices   = nullptr;
    }

    mSwapchain = mLogicalDevice.CreateSwapchain(lSwapchainCreateInfo);

    if (lSwapchainCreateInfo.oldSwapchain != VK_NULL_HANDLE)
    {
        mLogicalDevice.Wait();
        DestroyBuffers();
        vkDestroySwapchainKHR(mLogicalDevice, lSwapchainCreateInfo.oldSwapchain, nullptr);
    }

    CreateBuffers(lSwapchainCreateInfo.imageFormat, kBGRA_8888_SkColorType, lExtent.width, lExtent.height);
}


void VulkanContext::CreateBuffers(const VkFormat aFormat, const SkColorType aColorType, const int aWidth, const int aHeight)
{
    mSwapchainImages = mLogicalDevice.GetSwapchainImages(mSwapchain);

    const auto lImageCount = mSwapchainImages.size();

    mImageLayouts.resize(lImageCount, VK_IMAGE_LAYOUT_UNDEFINED);
    mSkSurfaces.resize(lImageCount);

    for (std::size_t lIndex = 0; lIndex < lImageCount; ++lIndex)
    {
        const GrVkImageInfo lInfo{mSwapchainImages[lIndex], GrVkAlloc(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_LAYOUT_UNDEFINED, aFormat, 1, mPresentQueue.familyIndex};

        if (mSampleCount == 1)
        {
            const GrBackendRenderTarget lBackendRenderTarget{aWidth, aHeight, mSampleCount, lInfo};
            mSkSurfaces[lIndex] = SkSurface::MakeFromBackendRenderTarget(mContext.get(), lBackendRenderTarget, kTopLeft_GrSurfaceOrigin, aColorType, mColorSpace, &mSurfaceProps);
        }
        else
        {
            const GrBackendTexture lBackendTexture{aWidth, aHeight, lInfo};
            mSkSurfaces[lIndex] =
                SkSurface::MakeFromBackendTexture(mContext.get(), lBackendTexture, kTopLeft_GrSurfaceOrigin, mSampleCount, aColorType, mColorSpace, &mSurfaceProps);
        }
    }

    constexpr VkSemaphoreCreateInfo lSemaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = nullptr, .flags = 0};

    mBackBuffers.resize(lImageCount + 1);

    for (auto& lBackBuffer : mBackBuffers)
        lBackBuffer.renderSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);

    mCurrentBackbufferIndex = static_cast<uint32_t>(lImageCount);
}


void VulkanContext::DestroyBuffers()
{
    for (const auto& lBackBuffer : mBackBuffers)
        mLogicalDevice.DestroySemaphore(lBackBuffer.renderSemaphore);
}


sk_sp<SkSurface> VulkanContext::GetBackBufferSurface()
{
    constexpr VkSemaphoreCreateInfo lSemaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = nullptr, .flags = 0};
    const auto lSemaphore = mLogicalDevice.CreateSemaphore(lSemaphoreCreateInfo);

    auto& lBackBuffer      = GetAvailBackbuffer();
    lBackBuffer.imageIndex = mLogicalDevice.AcquireNextImage(mSwapchain, lSemaphore);

    auto lSkSurface = mSkSurfaces[lBackBuffer.imageIndex].get();

    GrBackendSemaphore lBackendSemaphore;
    lBackendSemaphore.initVulkan(lSemaphore);

    lSkSurface->wait(1, &lBackendSemaphore);

    return sk_ref_sp(lSkSurface);
}


void VulkanContext::SwapBuffers()
{
    const auto lBackbuffer = mBackBuffers[mCurrentBackbufferIndex];

    GrBackendSemaphore lBackendSemaphore;
    lBackendSemaphore.initVulkan(lBackbuffer.renderSemaphore);

    const GrFlushInfo lFlushInfo{.fNumSemaphores = 1, .fSignalSemaphores = &lBackendSemaphore};

    const auto lSkSurface = mSkSurfaces[lBackbuffer.imageIndex].get();
    lSkSurface->flush(SkSurface::BackendSurfaceAccess::kPresent, lFlushInfo);

    const VkPresentInfoKHR lPresentInfo{.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                        .pNext              = nullptr,
                                        .waitSemaphoreCount = 1,
                                        .pWaitSemaphores    = &lBackbuffer.renderSemaphore,
                                        .swapchainCount     = 1,
                                        .pSwapchains        = &mSwapchain,
                                        .pImageIndices      = &lBackbuffer.imageIndex,
                                        .pResults           = nullptr};

    mPresentQueue.handle.Present(lPresentInfo);
}



}    // namespace abollo