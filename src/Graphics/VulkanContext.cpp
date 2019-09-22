#include "Graphics/VulkanContext.h"



namespace abollo
{



void VulkanContext::CreateInstance(const std::string_view aAppName, const uint32_t aAppVersion, const std::string_view aEngineName, const uint32_t aEngineVersion)
{
    VkApplicationInfo lApplicationInfo{.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                       .pNext              = nullptr,
                                       .pApplicationName   = aAppName.data(),
                                       .applicationVersion = aAppVersion,
                                       .pEngineName        = aEngineName.data(),
                                       .engineVersion      = aEngineVersion,
                                       .apiVersion         = VK_VERSION_1_1};
}


VulkanContext::VulkanContext(const std::string_view aAppName, const uint32_t aAppVersion, const std::string_view aEngineName, const uint32_t aEngineVersion)
{
}



}    // namespace abollo