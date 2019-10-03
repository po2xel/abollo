#ifndef __ABOLLO_GRAPHICS_VK_QUEUE_H__
#define __ABOLLO_GRAPHICS_VK_QUEUE_H__



#include <algorithm>

#include "vk.h"



namespace abollo::vk
{


class Queue final
{
private:
    VkQueue mQueue{VK_NULL_HANDLE};

public:
    Queue() noexcept = default;

    Queue(const VkQueue aQueue) noexcept : mQueue{aQueue}
    {
    }

    Queue(Queue&& aQueue) noexcept : mQueue{aQueue.mQueue}
    {
        aQueue.mQueue = VK_NULL_HANDLE;
    }

    void operator=(Queue&& aQueue) noexcept
    {
        std::swap(mQueue, aQueue.mQueue);
    }

    constexpr operator const VkQueue() const
    {
        return mQueue;
    }

    void Wait() const
    {
        ThrowIfFailed(vkQueueWaitIdle(mQueue));
    }

    void Present(const VkPresentInfoKHR& aPresentInfo) const
    {
        ThrowIfFailed(vkQueuePresentKHR(mQueue, &aPresentInfo));
    }
};



}    // namespace abollo::vk



#endif    // __ABOLLO_GRAPHICS_VK_QUEUE_H__
