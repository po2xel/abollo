#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>
#endif

#include <iostream>

#include <fmt/format.h>

#include "Window/Application.h"
#include "Window/Window.h"



using abollo::Application;
using abollo::KeyEvent;
using abollo::MouseEvent;
using abollo::Key;
using abollo::MouseMask;
using abollo::SubSystem;
using abollo::Window;



int main(int /*argc*/, char* /*argv*/[])
{
    auto& lApp = Application::Instance(SubSystem::eVideo);
    Window lWindow{"Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_RESIZABLE};

    lApp.Register(lWindow);

    // lWindow.On<MouseEvent::eLButtonDown>([](const Sint32 aPosX, const Sint32 aPosY) { std::cout << "Left button down: " << aPosX << "," << aPosY << "\n"; });
    // lWindow.On<MouseEvent::eLButtonUp>([](const Sint32 aPosX, const Sint32 aPosY) { std::cout << "Left button up: " << aPosX << "," << aPosY << "\n"; });
    /*lWindow.On<MouseEvent::eMotion>([](const Sint32 aPosX, const Sint32 aPosY, const Sint32 aPosRelX, const Sint32 aPosRelY, const Uint32 aMask) {
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
    });*/

    lWindow.On<MouseEvent::eWheel>([](const Sint32 aPosX, const Sint32 aPosY) { fmt::print("Mouse wheel: {}, {}\n", aPosX, aPosY); });

    lWindow.On<KeyEvent::eDown>([](const Key aKey, const Uint16 aModifier)
    {
        fmt::print("key pressed: {} -> {}\n", aKey, aModifier);
    });

    /*lWindow.Set<Event::eMouseButtonDown>(
        [](auto&& e) { std::cout << "Mouse Down: " << e.x << "\t" << e.y << std::endl; });
    lWindow.Set<Event::eMouseButtonUp>([](auto&& e) { std::cout << "Mouse Up: " << e.x << "\t" << e.y << std::endl; });

    lWindow.Set<Event::eKeyDown>(
        [](auto&& e) { std::cout << "Key down: " << e.keysym.scancode << "\t" << e.keysym.sym << std::endl; });

    lWindow.Set<Event::eKeyUp>(
        [](auto&& e) { std::cout << "Key up: " << e.keysym.scancode << "\t" << e.keysym.sym << std::endl; });*/

    lApp.Run();

    return 0;
}
