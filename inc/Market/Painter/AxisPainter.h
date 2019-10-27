#ifndef __ABOLLO_MARKET_PAINTER_AXIS_PAINTER_H__
#define __ABOLLO_MARKET_PAINTER_AXIS_PAINTER_H__



#include <limits>
#include <type_traits>

#include <fmt/format.h>

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/core/SkPaint.h>



namespace abollo
{



namespace axis
{



struct Date;
struct Price;
struct Volume;


struct Left;
struct Right;
struct Top;
struct Bottom;


}    // namespace axis



template <typename T, typename Tag>
struct Axis
{
    using type = Tag;

    T min{std::numeric_limits<T>::min()};
    T max{std::numeric_limits<T>::max()};
    SkScalar scale{1.f};
    SkScalar trans{0.f};

    [[nodiscard]] constexpr T stride() const
    {
        assert(max >= min);

        return max - min;
    }
};



class AxisPainter
{
private:
    constexpr static std::string_view DEFAULT_LABEL_FORMAT     = "{00000.00}";
    constexpr static std::string_view DEFAULT_LABEL_FORMAT_STR = "{:>8.2f}";

    SkFont mLabelFont;
    SkPaint mPaint;

    SkScalar mLabelWidth;
    SkScalar mLabelHeight;
    SkScalar mLabelSpace;

public:
    AxisPainter()
    {
        mPaint.setAntiAlias(true);
        mPaint.setColor(SK_ColorWHITE);

        const char* lFontFamily = nullptr;    // Default system family, if it exists.
        const SkFontStyle lFontStyle;         // Default is normal weight, normal width,  upright slant.
        const auto lFontManager = SkFontMgr::RefDefault();
        const auto lTypeface    = lFontManager->legacyMakeTypeface(lFontFamily, lFontStyle);

        mLabelFont.setTypeface(lTypeface);
        mLabelFont.setSize(12.f);
        mLabelFont.setScaleX(1.0f);
        mLabelFont.setSkewX(0.f);
        mLabelFont.setEdging(SkFont::Edging::kAntiAlias);

        SkRect lPriceLabelBound{};
        mLabelWidth  = mLabelFont.measureText(DEFAULT_LABEL_FORMAT.data(), DEFAULT_LABEL_FORMAT.size(), SkTextEncoding::kUTF8, &lPriceLabelBound);
        mLabelHeight = lPriceLabelBound.height();
        mLabelSpace  = mLabelHeight + mLabelHeight;
    }

    template <typename Pos, typename T, typename Tag>
    void Draw(SkCanvas& aCanvas, const Axis<T, Tag>& aAxis, const uint32_t aCount) const
    {
        const auto lCanvasClipBounds = aCanvas.getDeviceClipBounds();
        const auto lCanvasWidth  = static_cast<SkScalar>(lCanvasClipBounds.width());     // canvas width
        const auto lCanvasHeight = static_cast<SkScalar>(lCanvasClipBounds.height());    // canvas height

        const auto lStep = aAxis.stride() / aCount;
        SkScalar lCoordX{0.f};
        SkScalar lCoordY{0.f};

        if constexpr (std::is_same_v<Pos, axis::Right>)
            lCoordX = lCanvasWidth - mLabelWidth;
        else if constexpr (std::is_same_v<Pos, axis::Top>)
            lCoordY += mLabelHeight;
        else if constexpr (std::is_same_v<Pos, axis::Bottom>)
            lCoordY = lCanvasHeight;

        for (auto lStart = aAxis.min; lStart <= aAxis.max; lStart += lStep)
        {
            if constexpr (std::is_same_v<Pos, axis::Right> || std::is_same_v<Pos, axis::Left>)
                lCoordY = lStart * aAxis.scale + aAxis.trans;
            else if constexpr (std::is_same_v<Pos, axis::Top> || std::is_same_v<Pos, axis::Bottom>)
                lCoordX = lStart * aAxis.scale + aAxis.trans;
            else
            {
                static_assert(false, "Axis position is not supported.");
            }

            const auto& lLabel = fmt::format(DEFAULT_LABEL_FORMAT_STR, std::expf(lStart));

            aCanvas.drawString(lLabel.data(), lCoordX, lCoordY, mLabelFont, mPaint);
        }
    }
};



}    // namespace abollo



#endif    // __ABOLLO_MARKET_PAINTER_AXIS_PAINTER_H__
