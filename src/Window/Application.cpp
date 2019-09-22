#include "Window/Application.h"

#include "Window/EventSlot.h"
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
        try
        {
            switch (lEvent.type)
            {
            case SDL_WINDOWEVENT:
            {
                const auto& lWindow = mWindows.at(lEvent.window.windowID);
                lWindow.OnWindowEvent(lEvent.window);
                break;
            }

            case SDL_MOUSEMOTION:
            {
                const auto& lWindow = mWindows.at(lEvent.motion.windowID);
                lWindow.OnMouseMotionEvent(lEvent.motion);
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            {
                const auto& lWindow = mWindows.at(lEvent.button.windowID);
                lWindow.OnMouseButtonDownEvent(lEvent.button);
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                const auto& lWindow = mWindows.at(lEvent.button.windowID);
                lWindow.OnMouseButtonUpEvent(lEvent.button);
                break;
            }

            case SDL_MOUSEWHEEL:
            {
                const auto& lWindow = mWindows.at(lEvent.wheel.windowID);
                lWindow.OnMouseWheelEvent(lEvent.wheel);
                break;
            }

            case SDL_KEYDOWN:
            {
                const auto& lWindow = mWindows.at(lEvent.key.windowID);
                lWindow.OnKeyDownEvent(lEvent.key);
                break;
            }

            case SDL_KEYUP:
            {
                const auto& lWindow = mWindows.at(lEvent.key.windowID);
                lWindow.OnKeyUpEvent(lEvent.key);
                break;
            }

            default:
                break;
            }
        }
        catch (const std::out_of_range&)
        {
        }
    }
}



}    // namespace abollo