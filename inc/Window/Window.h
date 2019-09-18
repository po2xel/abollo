#ifndef __ABOLLO_WINDOW_WINDOW_H__
#define __ABOLLO_WINDOW_WINDOW_H__


#include <SDL2/SDL.h>
#include <functional>
#include <memory>
#include <string_view>

#include "NonCopyable.h"
#include "Typedef.h"



namespace abollo
{



class Window final : public internal::NonCopyable
{
private:
    Uint32 mWindowId;
    std::unique_ptr<WindowHandle, decltype(&SDL_DestroyWindow)> mWindow;

    std::function<void(const WindowEvent&)> mWindowEventHandler = [](const WindowEvent&) {};

    std::function<void(const KeyboardEvent&)> mKeyDownEventHandler = [](const KeyboardEvent&) {};
    std::function<void(const KeyboardEvent&)> mKeyUpEventHandler   = [](const KeyboardEvent&) {};

    std::function<void(const MouseMotionEvent&)> mMouseMotionHandler     = [](const MouseMotionEvent&) {};
    std::function<void(const MouseButtonEvent&)> mMouseButtonDownHandler = [](const MouseButtonEvent&) {};
    std::function<void(const MouseButtonEvent&)> mMouseButtonUpHandler   = [](const MouseButtonEvent&) {};
    std::function<void(const MouseWheelEvent&)> mMouseWheelEventHandler   = [](const MouseWheelEvent&) {};
    std::function<void(const MouseWheelEvent&)> mMouseWheelHandler       = [](const MouseWheelEvent&) {};

public:
    Window(const std::string_view aTitle, const int aPosX, const int aPosY, const int aWidth, const int aHeight,
           const Uint32 aFlags)
        : mWindow(SDL_CreateWindow(aTitle.data(), aPosX, aPosY, aWidth, aHeight, aFlags), &SDL_DestroyWindow)
    {
        mWindowId = SDL_GetWindowID(mWindow.get());
    }

    ~Window()
    {
        mWindow = nullptr;
    }

    [[nodiscard]] Uint32 GetWindowId() const
    {
        return mWindowId;
    }


    [[nodiscard]] WindowHandle* GetHandle() const
    {
        return mWindow.get();
    }

    template <Event e, typename E>
    constexpr void On(E&& aEvent) const
    {
        if constexpr (e == Event::eWindowEvent)
            mWindowEventHandler(aEvent);
        else if constexpr (e == Event::eMouseMotion)
            mMouseMotionHandler(aEvent);
        else if constexpr (e == Event::eMouseButtonDown)
            mMouseButtonDownHandler(aEvent);
        else if constexpr (e == Event::eMouseButtonUp)
            mMouseButtonUpHandler(aEvent);
        else if constexpr (e == Event::eMouseWheel)
            mMouseWheelEventHandler(aEvent);
        else if constexpr (e == Event::eKeyDown)
            mKeyDownEventHandler(aEvent);
        else if constexpr (e == Event::eKeyUp)
            mKeyUpEventHandler(aEvent);
        else
            static_assert(false, "Event type is not supported.");

        /*switch (e)
        {
        case Event::eWindowEvent:
            mWindowEventHandler(aEvent);
            break;

        case Event::eMouseMotion:
            mMouseMotionHandler(aEvent);
            break;

        default:
            static_assert(false, "Event type is not supported.");
        }*/
    }

    template <Event e, typename Callable>
    constexpr void Set(Callable&& aEventHandler)
    {
        if constexpr (e == Event::eWindowEvent)
            mWindowEventHandler = aEventHandler;
        else if constexpr (e == Event::eMouseMotion)
            mMouseMotionHandler = aEventHandler;
        else if constexpr (e == Event::eMouseButtonDown)
            mMouseButtonDownHandler = aEventHandler;
        else if constexpr (e == Event::eMouseButtonUp)
            mMouseButtonUpHandler = aEventHandler;
        else if constexpr (e == Event::eMouseWheel)
            mMouseWheelEventHandler = aEventHandler;
        else if constexpr (e == Event::eKeyDown)
            mKeyDownEventHandler = aEventHandler;
        else if constexpr (e == Event::eKeyUp)
            mKeyUpEventHandler = aEventHandler;
        else
            static_assert(false, "Event type is not supported.");

        /*switch (e)
        {
        case Event::eWindowEvent:
            mWindowEventHandler = aEventHandler;
            break;

        case Event::eMouseMotion:
            mMouseMotionHandler = aEventHandler;
            break;

        default:
            static_assert(false, "Event type is not supported.");
        }*/
    }
};



}    // namespace abollo



#endif    // !__ABOLLO_WINDOW_WINDOW_H__
