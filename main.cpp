// #ifdef _DEBUG
// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>
// #include <cstdlib>
// #endif

#include <iostream>

#include <date/date.h>
#include <fmt/format.h>

#include <thrust/sequence.h>

#include "Graphics/VulkanContext.h"
#include "Market/MarketCanvas.h"
#include "Window/Application.h"
#include "Window/Event.h"
#include "Window/EventSlot.h"
#include "Window/Window.h"



using abollo::Application;
using abollo::CursorType;
using abollo::Event;
using abollo::Key;
using abollo::KeyEvent;
using abollo::MarketCanvas;
using abollo::MouseEvent;
using abollo::MouseMask;
using abollo::SubSystem;
using abollo::VulkanContext;
using abollo::Window;
using abollo::WindowEvent;



int main(int /*argc*/, char* /*argv*/[])
{
    auto& lApp = Application::Instance(SubSystem::eVideo);
    const Window lWindow{"Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN};

    Event<MouseEvent::eLButtonDown, MouseEvent::eLButtonUp, MouseEvent::eRButtonDown, MouseEvent::eRButtonUp, MouseEvent::eMotion, MouseEvent::eWheel, KeyEvent::eDown,
          KeyEvent::eUp, WindowEvent::eShown, WindowEvent::eMoved, WindowEvent::eResized, WindowEvent::eSizeChanged, WindowEvent::eEnter, WindowEvent::eLeave>
        lEvents;

    lApp.Bind(lWindow.GetWindowId(), lEvents);

    VulkanContext lVulkanContext{lWindow, "Hello World", 1, "", 0};

    const auto& lExtent = lVulkanContext.GetExtent();
    MarketCanvas lMarketCanvas{lExtent.width, lExtent.height};

    lEvents.On<MouseEvent::eLButtonDown>([&lMarketCanvas](const Sint32 aPosX, const Sint32 aPosY) {
        // lMarketCanvas.ResetMode(CanvasMode::eTrendLine);
        // lMarketCanvas.LButtonDown(static_cast<SkScalar>(aPosX), static_cast<SkScalar>(aPosY));

        lMarketCanvas.Begin(static_cast<SkScalar>(aPosX), static_cast<SkScalar>(aPosY));
    });

    lEvents.On<MouseEvent::eLButtonUp>([&lMarketCanvas](const Sint32 aPosX, const Sint32 aPosY) {
        // lMarketCanvas.ResetMode();
        // lMarketCanvas.LButtonUp(static_cast<SkScalar>(aPosX), static_cast<SkScalar>(aPosY));

        lMarketCanvas.End(static_cast<SkScalar>(aPosX), static_cast<SkScalar>(aPosY));
    });

    lEvents.On<MouseEvent::eRButtonDown>([&lApp](const Sint32 /*aPosX*/, const Sint32 /*aPosY*/) { lApp.SetCursor(CursorType::eHand); });
    lEvents.On<MouseEvent::eRButtonUp>([&lApp](const Sint32 /*aPosX*/, const Sint32 /*aPosY*/) { lApp.SetCursor(CursorType::eArrow); });

    lEvents.On<WindowEvent::eShown>([&lVulkanContext, &lMarketCanvas] {
        const auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketCanvas.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<WindowEvent::eResized>([&lVulkanContext, &lMarketCanvas](const Sint32 /*aWidth*/, const Sint32 /*aHeight*/) {
        lVulkanContext.CreateSwapchain();

        if (const auto lBackBuffer = lVulkanContext.GetBackBufferSurface(); lBackBuffer)
        {
            lMarketCanvas.Resize();

            lMarketCanvas.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<MouseEvent::eMotion>([&lVulkanContext, &lMarketCanvas](const Sint32 aPosX, const Sint32 aPosY, const Sint32 aPosRelX, const Sint32 aPosRelY, const Uint32 aMask) {
        lMarketCanvas.Pick(static_cast<SkScalar>(aPosX), static_cast<SkScalar>(aPosY));

        if ((aMask & MouseMask::eRight) == MouseMask::eRight)
            lMarketCanvas.Pan(static_cast<SkScalar>(aPosRelX), static_cast<SkScalar>(aPosRelY));
        else if ((aMask & MouseMask::eLeft) == MouseMask::eLeft)
            lMarketCanvas.Next(static_cast<SkScalar>(aPosX), static_cast<SkScalar>(aPosY));
        // else
        // return;

        const auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketCanvas.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<MouseEvent::eWheel>([&lVulkanContext, &lMarketCanvas](const Sint32 aScrolledX, const Sint32 aScrolledY) {
        lMarketCanvas.Zoom(static_cast<SkScalar>(aScrolledX), static_cast<SkScalar>(aScrolledY));

        const auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketCanvas.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<KeyEvent::eDown>([&lVulkanContext, &lMarketCanvas, &lWindow](const Key aKey, const Uint16 /*aModifier*/) {
        switch (aKey)
        {
            // case Key::ePrintScreen:
            // {
            //     const auto lBackBuffer = lVulkanContext.GetBackBufferSurface();
            //     lMarketCanvas.Capture(lBackBuffer.get());
            //     break;
            // }

        case Key::eL:
            lMarketCanvas.ResetMode<abollo::TrendLine>();
            break;

        case Key::eF:
            lMarketCanvas.ResetMode<abollo::FibRetracement>();
            break;

        case Key::eEsc:
            // lMarketCanvas.Begin<abollo::fib_retracement_tag>();
            lMarketCanvas.ResetMode();
            break;

        case Key::eLeft:
        {
            int x, y;
            SDL_GetMouseState(&x, &y);

            SDL_WarpMouseInWindow(lWindow.GetHandle(), x - 1, y);
            break;
        }

        case Key::eRight:
        {
            int x, y;
            SDL_GetMouseState(&x, &y);

            SDL_WarpMouseInWindow(lWindow.GetHandle(), x + 1, y);

            break;
        }

        case Key::eUp:
        {
            int x, y;
            SDL_GetMouseState(&x, &y);

            SDL_WarpMouseInWindow(lWindow.GetHandle(), x, y - 1);
            break;
        }

        case Key::eDown:
        {
            int x, y;
            SDL_GetMouseState(&x, &y);

            SDL_WarpMouseInWindow(lWindow.GetHandle(), x, y + 1);

            break;
        }

        default:
            break;
        }
    });

    lApp.Run();

    return 0;
}
