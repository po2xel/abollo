#include "Window/Application.h"

#include "Window/EventSlot.h"



namespace abollo
{



void Application::Bind(const Uint32 aWindowId, const EventDispatcher& aEventDispatcher)
{
    mEventDispatchers.emplace(aWindowId, aEventDispatcher);
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
                const auto& lEventDispatcher = mEventDispatchers.at(lEvent.window.windowID);
                lEventDispatcher.OnWindowEvent(lEvent.window);
                break;
            }

            case SDL_MOUSEMOTION:
            {
                const auto& lEventDispatcher = mEventDispatchers.at(lEvent.motion.windowID);
                lEventDispatcher.OnMouseMotionEvent(lEvent.motion);
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            {
                const auto& lEventDispatcher = mEventDispatchers.at(lEvent.button.windowID);
                lEventDispatcher.OnMouseButtonDownEvent(lEvent.button);
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                const auto& lEventDispatcher = mEventDispatchers.at(lEvent.button.windowID);
                lEventDispatcher.OnMouseButtonUpEvent(lEvent.button);
                break;
            }

            case SDL_MOUSEWHEEL:
            {
                const auto& lEventDispatcher = mEventDispatchers.at(lEvent.wheel.windowID);
                lEventDispatcher.OnMouseWheelEvent(lEvent.wheel);
                break;
            }

            case SDL_KEYDOWN:
            {
                const auto& lEventDispatcher = mEventDispatchers.at(lEvent.key.windowID);
                lEventDispatcher.OnKeyDownEvent(lEvent.key);
                break;
            }

            case SDL_KEYUP:
            {
                const auto& lEventDispatcher = mEventDispatchers.at(lEvent.key.windowID);
                lEventDispatcher.OnKeyUpEvent(lEvent.key);
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