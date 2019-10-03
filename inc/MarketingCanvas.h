#ifndef __ABOLLO_MARKETING_CANVAS_H__
#define __ABOLLO_MARKETING_CANVAS_H__



#include <skia/include/core/SkSurface.h>



namespace abollo
{



class MarketingCanvas final
{
public:
    void Paint(SkSurface* aSurface);
};



}    // namespace abollo




#endif    // __ABOLLO_MARKETING_CANVAS_H__
