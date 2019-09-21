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



enum class MessageType : Uint8
{
    eError   = SDL_MESSAGEBOX_ERROR,
    eWarning = SDL_MESSAGEBOX_WARNING,
    eInfo    = SDL_MESSAGEBOX_INFORMATION
};



class Window final : private internal::NonCopyable
{
private:
    Uint32 mWindowId;
    std::unique_ptr<WindowHandle, decltype(&SDL_DestroyWindow)> mWindow;

    WindowEventSlot mWindowEventSlot;
    MouseEventSlot mMouseEventSlot;
    KeyEventSlot mKeyEventSlot;

    friend class Application;


    template <auto E, typename... Args>
    constexpr void Signal(Args&&... aArgs) const
    {
        using EventType = decltype(E);

        if constexpr (std::is_same_v<EventType, WindowEvent>)
            mWindowEventSlot.Signal<E>(std::forward<Args>(aArgs)...);
        else if constexpr (std::is_same_v<EventType, MouseEvent>)
            mMouseEventSlot.Signal<E>(std::forward<Args>(aArgs)...);
        else if constexpr (std::is_same_v<EventType, KeyEvent>)
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

    [[nodiscard]] std::pair<int, int> GetPos() const
    {
        auto lPosX = 0, lPosY = 0;

        SDL_GetWindowPosition(mWindow.get(), &lPosX, &lPosY);

        return {lPosX, lPosY};
    }

    void SetPos(const int aPosX, const int aPosY) const
    {
        SDL_SetWindowPosition(mWindow.get(), aPosX, aPosY);
    }

    [[nodiscard]] std::pair<int, int> GetSize() const
    {
        auto lWidth = 0, lHeight = 0;

        SDL_GetWindowSize(mWindow.get(), &lWidth, &lHeight);

        return {lWidth, lHeight};
    }

    void SetSize(const int aWidth, const int aHeight) const
    {
        SDL_SetWindowSize(mWindow.get(), aWidth, aHeight);
    }

    [[nodiscard]] std::string_view GetTitle() const
    {
        return SDL_GetWindowTitle(mWindow.get());
    }

    void SetTitle(const std::string_view aTitle) const
    {
        SDL_SetWindowTitle(mWindow.get(), aTitle.data());
    }

    [[nodiscard]] float GetBrightness() const
    {
        return SDL_GetWindowBrightness(mWindow.get());
    }

    bool SetBrightness(const float aBrightness) const
    {
        return 0 == SDL_SetWindowBrightness(mWindow.get(), aBrightness);
    }

    [[nodiscard]] int GetDisplayIndex() const
    {
        return SDL_GetWindowDisplayIndex(mWindow.get());
    }

    [[nodiscard]] Uint32 GetFlags() const
    {
        return SDL_GetWindowFlags(mWindow.get());
    }

    void Show() const
    {
        SDL_ShowWindow(mWindow.get());
    }

    void Hide() const
    {
        SDL_HideWindow(mWindow.get());
    }

    void Maximize() const
    {
        SDL_MaximizeWindow(mWindow.get());
    }

    void Minimize() const
    {
        SDL_MinimizeWindow(mWindow.get());
    }

    void Raise() const
    {
        SDL_RaiseWindow(mWindow.get());
    }

    void Restore() const
    {
        SDL_RestoreWindow(mWindow.get());
    }

    void RemoveBorder() const
    {
        SDL_SetWindowBordered(mWindow.get(), SDL_FALSE);
    }

    void AddBorder() const
    {
        SDL_SetWindowBordered(mWindow.get(), SDL_TRUE);
    }

    bool IsGrabbed() const
    {
        return SDL_TRUE == SDL_GetWindowGrab(mWindow.get());
    }

    void GrabInput() const
    {
        SDL_SetWindowGrab(mWindow.get(), SDL_TRUE);
    }

    void ReleaseInput() const
    {
        SDL_SetWindowGrab(mWindow.get(), SDL_FALSE);
    }

    bool SetInputFocus() const
    {
        return 0 == SDL_SetWindowInputFocus(mWindow.get());
    }

    bool SetOpacity(const float aOpacity) const
    {
        return 0 == SDL_SetWindowOpacity(mWindow.get(), aOpacity);
    }

    bool ShowMessage(const MessageType aType, const std::string_view aTitle, const std::string_view aMessage)
    {
        using Underlying = std::underlying_type_t<MessageType>;

        return 0 == SDL_ShowSimpleMessageBox(static_cast<Underlying>(aType), aTitle.data(), aMessage.data(), mWindow.get());
    }

    template <auto E, typename Slot>
    constexpr void On(Slot&& aSlot)
    {
        using EventType = decltype(E);

        if constexpr (std::is_same_v<EventType, WindowEvent>)
            mWindowEventSlot.On<E>(std::forward<Slot>(aSlot));
        else if constexpr (std::is_same_v<EventType, MouseEvent>)
            mMouseEventSlot.On<E>(std::forward<Slot>(aSlot));
        else if constexpr (std::is_same_v<EventType, KeyEvent>)
            mKeyEventSlot.On<E>(std::forward<Slot>(aSlot));
    }
};



}    // namespace abollo



#endif    // !__ABOLLO_WINDOW_WINDOW_H__
