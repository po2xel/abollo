// #ifdef _DEBUG
// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>
// #include <cstdlib>
// #endif

#include <iostream>

#include <fmt/format.h>

#include "Graphics/VulkanContext.h"
#include "MarketingPainter.h"
#include "Window/Application.h"
#include "Window/Event.h"
#include "Window/EventSlot.h"
#include "Window/Window.h"



using abollo::Application;
using abollo::CursorType;
using abollo::Event;
using abollo::Key;
using abollo::KeyEvent;
using abollo::MarketingPainter;
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

    VulkanContext lVulkanContext{lWindow, "Hello World", 1, "", 0};

    Event<MouseEvent::eLButtonDown, MouseEvent::eLButtonUp, MouseEvent::eRButtonDown, MouseEvent::eMotion, MouseEvent::eWheel, KeyEvent::eDown, KeyEvent::eUp, WindowEvent::eShown,
          WindowEvent::eMoved, WindowEvent::eResized, WindowEvent::eSizeChanged, WindowEvent::eEnter, WindowEvent::eLeave>
        lEvents;

    lApp.Bind(lWindow.GetWindowId(), lEvents);

    lEvents.On<MouseEvent::eLButtonDown>([&lApp](const Sint32 aPosX, const Sint32 aPosY) {
        std::cout << "Left button down: " << aPosX << "," << aPosY << "\n";
        lApp.SetCursor(CursorType::eHand);
    });

    lEvents.On<MouseEvent::eLButtonUp>([&lApp](const Sint32 aPosX, const Sint32 aPosY) {
        std::cout << "Right button up: " << aPosX << "," << aPosY << "\n";

        lApp.SetCursor(CursorType::eArrow);
    });

    MarketingPainter lMarketingPainter;

    lEvents.On<WindowEvent::eShown>([&lVulkanContext, &lMarketingPainter] {
        auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketingPainter.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<WindowEvent::eResized>([&lVulkanContext, &lMarketingPainter](const Sint32 aWidth, const Sint32 aHeight) {
        fmt::print("Window resized to {}, {}\n", aWidth, aHeight);

        lVulkanContext.CreateSwapchain();

        auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketingPainter.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<MouseEvent::eMotion>(
        [&lVulkanContext, &lMarketingPainter](const Sint32 aPosX, const Sint32 aPosY, const Sint32 aPosRelX, const Sint32 aPosRelY, const Uint32 aMask) {
            if ((aMask & MouseMask::eLeft) != MouseMask::eLeft)
            {
                lMarketingPainter.Move(static_cast<SkScalar>(aPosX), static_cast<SkScalar>(aPosY));

                return;
            }

            fmt::print("Mouse moved to: {}, {}\n", aPosRelX, aPosRelY);

            lMarketingPainter.MoveTo(static_cast<SkScalar>(aPosRelX), static_cast<SkScalar>(aPosRelY));

            auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

            if (lBackBuffer)
            {
                lMarketingPainter.Paint(lBackBuffer.get());
                lBackBuffer->flush();
                lVulkanContext.SwapBuffers();
            }
        });

    lEvents.On<MouseEvent::eWheel>([&lVulkanContext, &lMarketingPainter](const Sint32 aScrolledX, const Sint32 aScrolledY) {
        fmt::print("Mouse Wheel: {}, {}\n", aScrolledX, aScrolledY);

        lMarketingPainter.Zoom(static_cast<SkScalar>(aScrolledX), static_cast<SkScalar>(aScrolledY));

        auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketingPainter.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lApp.Run();

    return 0;
}
