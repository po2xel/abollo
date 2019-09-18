#include "Window/Application.h"

#include "Window/Window.h"



namespace abollo
{



void Application::Register(const Window& aWindow)
{
    mWindows.emplace(aWindow.GetWindowId(), aWindow);
}


void Application::Run() const
{
    for (SDL_Event lEvent; SDL_WaitEvent(&lEvent) && lEvent.type != SDL_QUIT;)
    {
        switch (lEvent.type)
        {
        case SDL_WINDOWEVENT:
            mWindows.at(lEvent.window.windowID).On<Event::eWindowEvent>(lEvent.window);
            break;

        case SDL_MOUSEMOTION:
            mWindows.at(lEvent.motion.windowID).On<Event::eMouseMotion>(lEvent.motion);
            break;

        case SDL_MOUSEBUTTONDOWN:
            mWindows.at(lEvent.button.windowID).On<Event::eMouseButtonDown>(lEvent.button);
            break;

        case SDL_MOUSEBUTTONUP:
            mWindows.at(lEvent.button.windowID).On<Event::eMouseButtonUp>(lEvent.button);
            break;

        case SDL_MOUSEWHEEL:
            mWindows.at(lEvent.wheel.windowID).On<Event::eMouseWheel>(lEvent.wheel);
            break;

        case SDL_KEYDOWN:
            mWindows.at(lEvent.key.windowID).On<Event::eKeyDown>(lEvent.key);
            break;

        case SDL_KEYUP:
            mWindows.at(lEvent.key.windowID).On<Event::eKeyUp>(lEvent.key);
            break;

        default:
            break;
        }
    }
}


}    // namespace abollo