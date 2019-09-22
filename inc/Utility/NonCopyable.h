#ifndef __ABOLLO_UTILITY_NONCOPYABLE_H__
#define __ABOLLO_UTILITY_NONCOPYABLE_H__



namespace abollo::internal
{



class NonCopyable
{
public:
    constexpr NonCopyable() noexcept = default;
    ~NonCopyable() noexcept          = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&)      = delete;

    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;
};



}    // namespace abollo::internal



#endif    // !__ABOLLO_UTILITY_NONCOPYABLE_H__
