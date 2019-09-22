#ifndef __ABOLLO_WINDOW_WINDOW_H__
#define __ABOLLO_WINDOW_WINDOW_H__


#include <SDL2/SDL.h>
#include <memory>
#include <string_view>



#include "Utility/NonCopyable.h"



namespace abollo
{



using WindowHandle = SDL_Window;



enum class MessageType : Uint8
{
    eError   = SDL_MESSAGEBOX_ERROR,
    eWarning = SDL_MESSAGEBOX_WARNING,
    eInfo    = SDL_MESSAGEBOX_INFORMATION
};



class Window : private internal::NonCopyable
{
private:
    Uint32 mWindowId;
    std::unique_ptr<WindowHandle, decltype(&SDL_DestroyWindow)> mWindow;

public:
    Window(const std::string_view aTitle, const int aPosX, const int aPosY, const int aWidth, const int aHeight, const Uint32 aFlags)
        : mWindow(SDL_CreateWindow(aTitle.data(), aPosX, aPosY, aWidth, aHeight, aFlags), &SDL_DestroyWindow)
    {
        mWindowId = SDL_GetWindowID(mWindow.get());
    }

    virtual ~Window()
    {
        mWindow = nullptr;
    }

    virtual void OnWindowEvent(const SDL_WindowEvent& aEvent) const = 0;

    virtual void OnMouseButtonDownEvent(const SDL_MouseButtonEvent& aEvent) const = 0;
    virtual void OnMouseButtonUpEvent(const SDL_MouseButtonEvent& aEvent) const   = 0;
    virtual void OnMouseMotionEvent(const SDL_MouseMotionEvent& aEvent) const     = 0;
    virtual void OnMouseWheelEvent(const SDL_MouseWheelEvent& aEvent) const       = 0;

    virtual void OnKeyDownEvent(const SDL_KeyboardEvent& aEvent) const = 0;
    virtual void OnKeyUpEvent(const SDL_KeyboardEvent& aEvent) const   = 0;

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

    [[nodiscard]] std::pair<int, int> GetDrawableSize() const
    {
        auto lWidth = 0, lHeight = 0;

        SDL_GL_GetDrawableSize(mWindow.get(), &lWidth, &lHeight);

        return {lWidth, lHeight};
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

    bool ShowMessage(const MessageType aType, const std::string_view aTitle, const std::string_view aMessage) const
    {
        using Underlying = std::underlying_type_t<MessageType>;

        return 0 == SDL_ShowSimpleMessageBox(static_cast<Underlying>(aType), aTitle.data(), aMessage.data(), mWindow.get());
    }
};



}    // namespace abollo



#endif    // !__ABOLLO_WINDOW_WINDOW_H__
