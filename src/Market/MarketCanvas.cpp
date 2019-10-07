#include "Market/MarketCanvas.h"

#include <fmt/format.h>

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/core/SkPath.h>
#include <skia/include/core/SkTextBlob.h>

#include "Market/Model/Index.h"



namespace abollo
{



MarketCanvas::MarketCanvas()
{
    mTradeDate.Load();
    mIndexData.Load(mTradeDate.front(), mTradeDate.back());

    mpMarketPainter = std::make_unique<Painter>();

    const char* fontFamily = nullptr;    // Default system family, if it exists.
    const SkFontStyle fontStyle;         // Default is normal weight, normal width,  upright slant.
    const auto fontManager = SkFontMgr::RefDefault();
    const auto typeface    = fontManager->legacyMakeTypeface(fontFamily, fontStyle);

    mAxisLabelFont.setTypeface(typeface);
    mAxisLabelFont.setSize(12.f);
    mAxisLabelFont.setScaleX(1.0f);
    mAxisLabelFont.setSkewX(0.f);
    mAxisLabelFont.setEdging(SkFont::Edging::kAntiAlias);

    mAxisPaint.setAntiAlias(true);
    mAxisPaint.setColor(SK_ColorWHITE);
}


void MarketCanvas::DrawAxes(SkCanvas& aCanvas, const Index& aPrices) const
{
    // const auto lRangeX = 20.f;    // range in x axis is: [-Inf., rangeX]
    // const auto lScaleX = 1024.f / lRangeX;

    auto i = 20;

    SkAutoCanvasRestore lGuard(&aCanvas, true);

    auto& lTransMatrix = aCanvas.getTotalMatrix();
    const auto lScaleX = lTransMatrix.getScaleX();
    const auto lTransX = lTransMatrix.getTranslateX();

    // lTransMatrix.setTranslateY(0.f);
    // lTransMatrix.setScaleY(1.f);

    // aCanvas.setMatrix(lTransMatrix);

    aCanvas.resetMatrix();

    for (const auto& lPrice : aPrices)
    {
        const auto& lDate = lPrice.date;
        const auto lLabel = fmt::format("{}/{}", static_cast<unsigned>(lDate.month()), static_cast<unsigned>(lDate.day()));
        // const auto lText  = SkTextBlob::MakeFromString(lLabel.data(), mAxisLabelFont);
        // aCanvas.drawTextBlob(lText.get(), 10.f, 768.f, mAxisPaint);

        const auto lLabelWidth = mAxisLabelFont.measureText(lLabel.data(), lLabel.size(), SkTextEncoding::kUTF8, nullptr);
        aCanvas.drawString(lLabel.data(), i * lScaleX + lTransX - lLabelWidth / 2.f, 768, mAxisLabelFont, mAxisPaint);

        --i;
    }
}


void MarketCanvas::Paint(SkSurface* apSurface) const
{
    auto lpCanvas = apSurface->getCanvas();

    lpCanvas->clear(SK_ColorDKGRAY);

    SkAutoCanvasRestore lGuard(lpCanvas, true);

    lpCanvas->concat(mTransMatrix);
    mpMarketPainter->Draw(*lpCanvas, mIndexData);

    DrawAxes(*lpCanvas, mIndexData);
}



}    // namespace abollo