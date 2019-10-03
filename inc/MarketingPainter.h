#ifndef __ABOLLO_MARKETING_PAINTER_H__
#define __ABOLLO_MARKETING_PAINTER_H__



#include <skia/include/core/SkSurface.h>

#include <iostream>



namespace abollo
{



class MarketingPainter final
{
private:
    SkMatrix mTransMatrix;
    // SkMatrix mTransMatrix2;
    SkScalar mMousePosX{0.f};
    SkScalar mMousePosY{0.f};

public:
    void Move(const SkScalar aPosX, const SkScalar aPosY)
    {
        mMousePosX = aPosX;
        mMousePosY = aPosY;
    }

    void MoveTo(const SkScalar aPosX, const SkScalar aPosY)
    {
        mTransMatrix.postTranslate(aPosX, aPosY);
    }

    void Zoom(const SkScalar /*aDeltaX*/, const SkScalar aDeltaY)
    {
        const auto lScaleX = 1.f + aDeltaY / 10.f;
        const auto lScaleY = 1.f + aDeltaY / 10.f;

        const auto lDeltaX = mMousePosX * (1.f - lScaleX);
        const auto lDeltaY = mMousePosY * (1.f - lScaleY);

        mTransMatrix.postConcat(SkMatrix::MakeAll(lScaleX, 0.f, lDeltaX, 0.f, lScaleY, lDeltaY, 0.f, 0.f, 1.f));
        // mTransMatrix.postTranslate(-mMousePosX, -mMousePosY).postScale(1.f + aDeltaY / 10.f, 1.f + aDeltaY / 10.f).postTranslate(mMousePosX, mMousePosY);
    }

    void Paint(SkSurface* aSurface);
};



}    // namespace abollo



#endif    // __ABOLLO_MARKETING_PAINTER_H__
