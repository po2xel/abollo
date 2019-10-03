// #ifdef _DEBUG
// #define _CRTDBG_MAP_ALLOC
// #include <crtdbg.h>
// #include <cstdlib>
// #endif

#include <iostream>

#include <fmt/format.h>

#include "Graphics/VulkanContext.h"
#include "MarketingCanvas.h"
#include "Window/Application.h"
#include "Window/Event.h"
#include "Window/EventSlot.h"
#include "Window/Window.h"



using abollo::Application;
using abollo::CursorType;
using abollo::Event;
using abollo::Key;
using abollo::KeyEvent;
using abollo::MarketingCanvas;
using abollo::MouseEvent;
using abollo::MouseMask;
using abollo::SubSystem;
using abollo::VulkanContext;
using abollo::Window;
using abollo::WindowEvent;



int main(int /*argc*/, char* /*argv*/[])
{
    auto& lApp = Application::Instance(SubSystem::eVideo);
    Window lWindow{"Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN};

    VulkanContext lVulkanContext{lWindow, "Hello World", 1, "", 0};

    Event<MouseEvent::eLButtonDown, MouseEvent::eRButtonDown, MouseEvent::eMotion, MouseEvent::eWheel, KeyEvent::eDown, KeyEvent::eUp, WindowEvent::eShown, WindowEvent::eMoved,
          WindowEvent::eResized, WindowEvent::eEnter>
        lEvents;

    /*std::cout << sizeof(lWindow) << std::endl;
    std::cout << sizeof(e) << std::endl;*/

    lApp.Bind(lWindow.GetWindowId(), lEvents);

    lEvents.On<MouseEvent::eLButtonDown>([&lWindow, &lApp](const Sint32 aPosX, const Sint32 aPosY) {
        std::cout << "Left button down: " << aPosX << "," << aPosY << "\n";
        lWindow.RemoveBorder();
        lApp.SetCursor(CursorType::eHand);

        // lWindow.Off<MouseEvent::eLButtonDown>();

        // lWindow.SetOpacity(0.5f);
        // lWindow.ShowMessage(abollo::MessageType::eError, "Title", "Message");
    });

    lEvents.On<MouseEvent::eLButtonUp>([&lWindow, &lApp](const Sint32 aPosX, const Sint32 aPosY) {
        std::cout << "Right button up: " << aPosX << "," << aPosY << "\n";
        lWindow.AddBorder();
        lApp.SetCursor(CursorType::eArrow);
    });

    /*lEvents.On<MouseEvent::eMotion>([](const Sint32 aPosX, const Sint32 aPosY, const Sint32 aPosRelX, const Sint32 aPosRelY, const Uint32 aMask) {
        fmt::print("Mouse moved to ({}, {}) -> ({}, {})\n", aPosX, aPosY, aPosRelX, aPosRelY);

        if ((aMask & MouseMask::eLeft) == MouseMask::eLeft)
            std::cout << "Left button is pressing.\n";

        if ((aMask & MouseMask::eRight) == MouseMask::eRight)
            std::cout << "Right button is pressing.\n";

        if ((aMask & MouseMask::eMiddle) == MouseMask::eMiddle)
            std::cout << "Middle button is pressing.\n";

        if ((aMask & MouseMask::eX1) == MouseMask::eX1)
            std::cout << "X1 button is pressing.\n";

        if ((aMask & MouseMask::eX2) == MouseMask::eX2)
            std::cout << "X2 button is pressing.\n";
    });

    lEvents.On<MouseEvent::eWheel>([](const Sint32 aPosX, const Sint32 aPosY) { fmt::print("Mouse wheel: {}, {}\n", aPosX, aPosY); });

    lEvents.On<KeyEvent::eDown>([](const Key aKey, const Uint16 aModifier) { fmt::print("key pressed: {} -> {}\n", aKey, aModifier); });
    lEvents.On<KeyEvent::eDown>([](const Key aKey, const Uint16 aModifier) { fmt::print("key pressed2: {} -> {}\n", aKey, aModifier); });
    lEvents.On<KeyEvent::eUp>([](const Key aKey, const Uint16 aModifier) { fmt::print("key released: {} -> {}\n", aKey, aModifier); });

    lEvents.On<WindowEvent::eMoved>([&lWindow](const Sint32, const Sint32) { std::cout << "Display index: " << lWindow.GetDisplayIndex() << std::endl; });
    lEvents.On<WindowEvent::eEnter>([]() { std::cout << "Entered\n"; });*/

    MarketingCanvas lMarketingCanvas;

    lEvents.On<WindowEvent::eShown>([&lVulkanContext, &lMarketingCanvas] {
        lVulkanContext.CreateSwapchain();

        auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketingCanvas.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    lEvents.On<WindowEvent::eResized>([&lVulkanContext, &lMarketingCanvas](const Sint32 aWidth, const Sint32 aHeight) {
        fmt::print("Window resized to {}, {}\n", aWidth, aHeight);

        lVulkanContext.CreateSwapchain();

        auto lBackBuffer = lVulkanContext.GetBackBufferSurface();

        if (lBackBuffer)
        {
            lMarketingCanvas.Paint(lBackBuffer.get());
            lBackBuffer->flush();
            lVulkanContext.SwapBuffers();
        }
    });

    /*const auto [lWidth, lHeight] = lWindow.GetSize();
    lWidth, lHeight;*/

    lApp.Run();

    return 0;
}
