#ifndef __ABOLLO_WINDOW_SINGLETON_H__
#define __ABOLLO_WINDOW_SINGLETON_H__



#include <utility>

#include "NonCopyable.h"



namespace abollo::internal
{



template <typename T>
class Singleton : public NonCopyable
{
public:
    template <typename... Args>
    static T& Instance(Args&&... aArgs)
    {
        static T lInstance{std::forward<Args>(aArgs)...};

        return lInstance;
    }
};



}    // namespace abollo::internal



#endif    // !__ABOLLO_WINDOW_SINGLETON_H__
