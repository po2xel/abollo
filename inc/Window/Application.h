#ifndef __ABOLLO_WINDOW_APPLICATION_H__
#define __ABOLLO_WINDOW_APPLICATION_H__



#include <stdexcept>
#include <unordered_map>

#include <SDL2/SDL.h>

#include "Singleton.h"



namespace abollo
{



class Window;



enum class SubSystem : Uint32
{
    eTimer          = SDL_INIT_TIMER,
    eAudio          = SDL_INIT_AUDIO,
    eVideo          = SDL_INIT_VIDEO,
    eJoyStick       = SDL_INIT_JOYSTICK,
    eHaptic         = SDL_INIT_HAPTIC,
    eGameController = SDL_INIT_GAMECONTROLLER,
    eEvents         = SDL_INIT_EVENTS,
    eSensor         = SDL_INIT_SENSOR,
    eNoParachute    = SDL_INIT_NOPARACHUTE,
    eEverything     = SDL_INIT_EVERYTHING
};


constexpr auto operator|(const SubSystem& aLhs, const SubSystem& aRhs)
{
    using Underlying = std::underlying_type_t<SubSystem>;

    return static_cast<SubSystem>(static_cast<Underlying>(aLhs) | static_cast<Underlying>(aRhs));
}



class Application final : private internal::Singleton<Application>
{
private:
    std::unordered_map<Uint32, const Window&> mWindows;

    void OnWindowEvent(const SDL_WindowEvent& aEvent) const;

    void OnMouseButtonDownEvent(const SDL_MouseButtonEvent& aEvent) const;
    void OnMouseButtonUpEvent(const SDL_MouseButtonEvent& aEvent) const;
    void OnMouseMotionEvent(const SDL_MouseMotionEvent& aEvent) const;
    void OnMouseWheelEvent(const SDL_MouseWheelEvent& aEvent) const;

    void OnKeyDownEvent(const SDL_KeyboardEvent& aEvent) const;
    void OnKeyUpEvent(const SDL_KeyboardEvent& aEvent) const;

public:
    using Singleton<Application>::Instance;

    explicit Application(const SubSystem& aFlags)
    {
        if (const auto ret = SDL_Init(static_cast<Uint32>(aFlags)); ret != 0)
            throw std::runtime_error("Failed to initialize SDL subsystems.");
    }

    ~Application()
    {
        SDL_Quit();
    }

    const Window& GetWindow(const Uint32 aWindowId)
    {
        return mWindows.at(aWindowId);
    }

    void Register(const Window& aWindow);
    void Run() const;


    static void DisableScreenSaver()    // Prevent the screen from being blanked by a screen saver.
    {
        SDL_DisableScreenSaver();
    }

    static void EnableScreenSaver()    // Allow the screen to be blanked by a screen saver.
    {
        SDL_EnableScreenSaver();
    }

    static bool IsScreenSaverEnabled()    // Return whether the screen saver is currently enabled. The screen saver is disabled by default.
    {
        return SDL_TRUE == SDL_IsScreenSaverEnabled();
    }
};



}    // namespace abollo



#endif    // !__ABOLLO_WINDOW_APPLICATION_H__
