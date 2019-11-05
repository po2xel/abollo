#ifndef __ABOLLO_UTILITY_MEDIAN_H__
#define __ABOLLO_UTILITY_MEDIAN_H__



#include <algorithm>



namespace abollo
{



template <typename T>
constexpr const T& Median(const T& a, const T& b, const T& c)
{
    return std::max(std::min(a, b), std::min(std::max(a, b), c));
}



}    // namespace abollo



#endif    // __ABOLLO_UTILITY_MEDIAN_H__
