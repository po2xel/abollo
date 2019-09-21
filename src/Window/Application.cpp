#include "Window/Application.h"

#include "Window/Window.h"



namespace abollo
{



void Application::OnMouseButtonDownEvent(const MouseButtonEvent& aEvent) const
{
    const auto& lWindow = mWindows.at(aEvent.windowID);

    switch (static_cast<MouseButton>(aEvent.button))
    {
    case MouseButton::eLeft:
        lWindow.Signal<MouseEvent::eLButtonDown>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eMiddle:
        lWindow.Signal<MouseEvent::eMButtonDown>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eRight:
        lWindow.Signal<MouseEvent::eRButtonDown>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eX1:
        lWindow.Signal<MouseEvent::eX1ButtonDown>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eX2:
        lWindow.Signal<MouseEvent::eX2ButtonDown>(aEvent.x, aEvent.y);
        break;
    }
}


void Application::OnMouseButtonUpEvent(const MouseButtonEvent& aEvent) const
{
    const auto& lWindow = mWindows.at(aEvent.windowID);

    switch (static_cast<MouseButton>(aEvent.button))
    {
    case MouseButton::eLeft:
        lWindow.Signal<MouseEvent::eLButtonUp>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eMiddle:
        lWindow.Signal<MouseEvent::eMButtonUp>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eRight:
        lWindow.Signal<MouseEvent::eRButtonUp>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eX1:
        lWindow.Signal<MouseEvent::eX1ButtonUp>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eX2:
        lWindow.Signal<MouseEvent::eX2ButtonUp>(aEvent.x, aEvent.y);
        break;
    }
}


void Application::OnMouseMotionEvent(const MouseMotionEvent& aEvent) const
{
    const auto& lWindow = mWindows.at(aEvent.windowID);

    lWindow.Signal<MouseEvent::eMotion>(aEvent.x, aEvent.y, aEvent.xrel, aEvent.yrel, aEvent.state);
}


void Application::OnMouseWheelEvent(const MouseWheelEvent& aEvent) const
{
    const auto& lWindow = mWindows.at(aEvent.windowID);

    lWindow.Signal<MouseEvent::eWheel>(aEvent.x, aEvent.y);
}


void Application::OnKeyDownEvent(const KeyboardEvent& aEvent) const
{
    const auto& lWindow = mWindows.at(aEvent.windowID);

    lWindow.Signal<KeyEvent::eDown>(static_cast<Key>(aEvent.keysym.sym), aEvent.keysym.mod);
}

void Application::OnKeyUpEvent(const KeyboardEvent& aEvent) const
{
    const auto& lWindow = mWindows.at(aEvent.windowID);

    lWindow.Signal<KeyEvent::eUp>(static_cast<Key>(aEvent.keysym.sym), aEvent.keysym.mod);
}


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
            /*case SDL_WINDOWEVENT:
                mWindows.at(lEvent.window.windowID).On<Event::eWindowEvent>(lEvent.window);
                break;*/

        case SDL_MOUSEMOTION:
            OnMouseMotionEvent(lEvent.motion);
            break;

        case SDL_MOUSEBUTTONDOWN:
            OnMouseButtonDownEvent(lEvent.button);
            break;

        case SDL_MOUSEBUTTONUP:
            OnMouseButtonUpEvent(lEvent.button);
            break;

        case SDL_MOUSEWHEEL:
            OnMouseWheelEvent(lEvent.wheel);
            break;

        case SDL_KEYDOWN:
            OnKeyDownEvent(lEvent.key);
            break;

        case SDL_KEYUP:
            OnKeyDownEvent(lEvent.key);
            break;

        default:
            break;
        }
    }
}



}    // namespace abollo