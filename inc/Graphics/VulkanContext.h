#ifndef __ABOLLO_GRAPHICS_VULKAN_CONTEXT_H__
#define __ABOLLO_GRAPHICS_VULKAN_CONTEXT_H__


#include <string_view>

#include <vulkan/vulkan.h>

#include "Utility/NonCopyable.h"



namespace abollo
{


class VulkanContext final : private internal::NonCopyable
{
private:
    VkInstance mInstance;

    void CreateInstance(const std::string_view aAppName, const uint32_t aAppVersion, const std::string_view aEngineName, const uint32_t aEngineVersion);

public:
    VulkanContext(const std::string_view aAppName, const uint32_t aAppVersion, const std::string_view aEngineName, const uint32_t aEngineVersion, const uint32_t aApiVersion);
};



}    // namespace abollo



#endif    // __ABOLLO_GRAPHICS_VULKAN_CONTEXT_H__
