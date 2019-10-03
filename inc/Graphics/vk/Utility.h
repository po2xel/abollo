#ifndef __ABOLLO_GRAPHICS_VK_UTILITY_H__
#define __ABOLLO_GRAPHICS_VK_UTILITY_H__



#include "vk.h"



namespace abollo::vk
{


inline void ThrowIfFailed(VkResult aResult)
{
    if (aResult != VK_SUCCESS)
        throw;
}



}    // namespace abollo::vk



#endif    // __ABOLLO_GRAPHICS_VK_UTILITY_H__
