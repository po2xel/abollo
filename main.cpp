#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cstdlib>
#endif

#include <iostream>

#include "Window/Application.h"
#include "Window/Typedef.h"
#include "Window/Window.h"



using abollo::Application;
using abollo::Event;
using abollo::SubSystem;
using abollo::Window;



int main(int /*argc*/, char* /*argv*/[])
{
    auto& lApp = Application::Instance(SubSystem::eVideo);
    Window lWindow{"Hello World", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, SDL_WINDOW_RESIZABLE};

    lApp.Register(lWindow);

    lWindow.Set<Event::eMouseButtonDown>(
        [](auto&& e) { std::cout << "Mouse Down: " << e.x << "\t" << e.y << std::endl; });
    lWindow.Set<Event::eMouseButtonUp>([](auto&& e) { std::cout << "Mouse Up: " << e.x << "\t" << e.y << std::endl; });

    lWindow.Set<Event::eKeyDown>(
        [](auto&& e) { std::cout << "Key down: " << e.keysym.scancode << "\t" << e.keysym.sym << std::endl; });

    lWindow.Set<Event::eKeyUp>(
        [](auto&& e) { std::cout << "Key up: " << e.keysym.scancode << "\t" << e.keysym.sym << std::endl; });

    lApp.Run();

    return 0;
}
