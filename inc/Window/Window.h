#ifndef __ABOLLO_WINDOW_WINDOW_H__
#define __ABOLLO_WINDOW_WINDOW_H__


#include <SDL2/SDL.h>
#include <memory>
#include <string_view>



#include "EventSlot.h"
#include "NonCopyable.h"



namespace abollo
{



using WindowHandle = SDL_Window;

class Application;



class Window final : private internal::NonCopyable
{
private:
    Uint32 mWindowId;
    std::unique_ptr<WindowHandle, decltype(&SDL_DestroyWindow)> mWindow;

    MouseEventSlot mMouseEventSlot;
    KeyEventSlot mKeyEventSlot;

    friend class Application;

    template <MouseEvent E, typename... Args>
    constexpr void Signal(Args&&... aArgs) const
    {
        mMouseEventSlot.Signal<E>(std::forward<Args>(aArgs)...);
    }

    template <KeyEvent E, typename... Args>
    constexpr void Signal(Args&&... aArgs) const
    {
        mKeyEventSlot.Signal<E>(std::forward<Args>(aArgs)...);
    }

public:
    Window(const std::string_view aTitle, const int aPosX, const int aPosY, const int aWidth, const int aHeight, const Uint32 aFlags)
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

    template <MouseEvent E, typename Slot>
    constexpr void On(Slot&& aSlot)
    {
        mMouseEventSlot.On<E>(std::forward<Slot>(aSlot));
    }

    template <KeyEvent E, typename Slot>
    constexpr void On(Slot&& aSlot)
    {
        mKeyEventSlot.On<E>(std::forward<Slot>(aSlot));
    }
};



}    // namespace abollo



#endif    // !__ABOLLO_WINDOW_WINDOW_H__
