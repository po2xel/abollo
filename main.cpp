// #ifdef _DEBUG
// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>
// #include <cstdlib>
// #endif

#include <iostream>

#include <date/date.h>
#include <fmt/format.h>

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

    Event<MouseEvent::eLButtonDown, MouseEvent::eLButtonUp, MouseEvent::eRButtonDown, MouseEvent::eMotion, MouseEvent::eWheel, KeyEvent::eDown, KeyEvent::eUp, WindowEvent::eShown,
          WindowEvent::eMoved, WindowEvent::eResized, WindowEvent::eSizeChanged, WindowEvent::eEnter, WindowEvent::eLeave>
        lEvents;

    lApp.Bind(lWindow.GetWindowId(), lEvents);

    lEvents.On<MouseEvent::eLButtonDown>([&lApp](const Sint32 /*aPosX*/, const Sint32 /*aPosY*/) { lApp.SetCursor(CursorType::eHand); });
    lEvents.On<MouseEvent::eLButtonUp>([&lApp](const Sint32 /*aPosX*/, const Sint32 /*aPosY*/) { lApp.SetCursor(CursorType::eArrow); });

    VulkanContext lVulkanContext{lWindow, "Hello World", 1, "", 0};

    MarketCanvas lMarketCanvas;

    lEvents.On<WindowEvent::eShown>([&lVulkanContext, &lMarketCanvas] {
        auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketCanvas.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<WindowEvent::eResized>([&lVulkanContext, &lMarketCanvas](const Sint32 /*aWidth*/, const Sint32 /*aHeight*/) {
        lVulkanContext.CreateSwapchain();

        auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketCanvas.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<MouseEvent::eMotion>([&lVulkanContext, &lMarketCanvas](const Sint32 aPosX, const Sint32 aPosY, const Sint32 aPosRelX, const Sint32 aPosRelY, const Uint32 aMask) {
        lMarketCanvas.Move(static_cast<SkScalar>(aPosX), static_cast<SkScalar>(aPosY));

        if ((aMask & MouseMask::eLeft) != MouseMask::eLeft)
            return;

        lMarketCanvas.MoveTo(static_cast<SkScalar>(aPosRelX), static_cast<SkScalar>(aPosRelY));

        auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketCanvas.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<MouseEvent::eWheel>([&lVulkanContext, &lMarketCanvas](const Sint32 aScrolledX, const Sint32 aScrolledY) {
        lMarketCanvas.Zoom(static_cast<SkScalar>(aScrolledX), static_cast<SkScalar>(aScrolledY));

        auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketCanvas.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<KeyEvent::eDown>([&lVulkanContext, &lMarketCanvas](const Key aKey, const Uint16 /*aModifier*/) {
        if (aKey == Key::ePrintScreen)
        {
            const auto lBackBuffer = lVulkanContext.GetBackBufferSurface();
            lMarketCanvas.Capture(lBackBuffer.get());
        }
    });

    lApp.Run();

    return 0;
}
