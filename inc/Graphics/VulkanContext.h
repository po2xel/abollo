#ifndef __ABOLLO_GRAPHICS_VULKAN_CONTEXT_H__
#define __ABOLLO_GRAPHICS_VULKAN_CONTEXT_H__


#include <string_view>

#include <skia/include/core/SkRefCnt.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkSurfaceProps.h>
#include <skia/include/core/SkTypes.h>
#include <skia/include/gpu/GrContext.h>
#include <skia/include/gpu/vk/GrVkTypes.h>

#include "Utility/NonCopyable.h"
#include "vk/Instance.h"



namespace abollo
{



class Window;



class VulkanContext final : private internal::NonCopyable
{
private:
    struct DeviceQueue
    {
        vk::Queue handle{VK_NULL_HANDLE};
        uint32_t familyIndex{UINT32_MAX};

        [[nodiscard]] bool IsValid() const
        {
            return familyIndex != UINT32_MAX;
        }
    };

    struct BackbufferInfo
    {
        uint32_t imageIndex{UINT32_MAX};
        VkSemaphore renderSemaphore;
    };

    vk::Instance mInstance;
    VkSurfaceKHR mSurface{VK_NULL_HANDLE};
    vk::PhysicalDevice mPhysicalDevice;

    DeviceQueue mGraphicsQueue;
    DeviceQueue mPresentQueue;

    vk::LogicalDevice mLogicalDevice;
    VkSwapchainKHR mSwapchain{VK_NULL_HANDLE};

    std::vector<VkImage> mSwapchainImages;
    std::vector<VkImageLayout> mImageLayouts;
    std::vector<sk_sp<SkSurface>> mSkSurfaces;
    std::vector<BackbufferInfo> mBackBuffers;

    sk_sp<GrContext> mContext;

    GrContextOptions mContextOptions;
    sk_sp<SkColorSpace> mColorSpace;
    SkSurfaceProps mSurfaceProps{SkSurfaceProps::kLegacyFontHost_InitType};

    uint32_t mCurrentBackbufferIndex{UINT32_MAX};
    int mSampleCount{1};

    void CreateBuffers(const VkFormat aFormat, const SkColorType aColorType, const int aWidth, const int aHeight);
    void DestroyBuffers();

    BackbufferInfo& GetAvailBackbuffer()
    {
        if (++mCurrentBackbufferIndex > mSwapchainImages.size())
            mCurrentBackbufferIndex = 0;

        return mBackBuffers[mCurrentBackbufferIndex];
    }


public:
    VulkanContext(const Window& aWindow, const std::string_view aAppName, const uint32_t aAppVersion, const std::string_view aEngineName, const uint32_t aEngineVersion);
    ~VulkanContext();

    void CreateSwapchain();

    sk_sp<SkSurface> GetBackBufferSurface();
    void SwapBuffers();
};



}    // namespace abollo



#endif    // __ABOLLO_GRAPHICS_VULKAN_CONTEXT_H__
