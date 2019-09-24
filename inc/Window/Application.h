#ifndef __ABOLLO_WINDOW_APPLICATION_H__
#define __ABOLLO_WINDOW_APPLICATION_H__



#include <memory>
#include <stdexcept>
#include <unordered_map>

#include <SDL2/SDL.h>

#include "Utility/Singleton.h"
#include "Window/EventDispatcher.h"



namespace abollo
{



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



enum class CursorType : Uint8
{
    eArrow     = SDL_SYSTEM_CURSOR_ARROW,
    eIBeam     = SDL_SYSTEM_CURSOR_IBEAM,
    eWait      = SDL_SYSTEM_CURSOR_WAIT,
    eCrossHair = SDL_SYSTEM_CURSOR_CROSSHAIR,
    eWaitArrow = SDL_SYSTEM_CURSOR_WAITARROW,
    eSizeNWSE  = SDL_SYSTEM_CURSOR_SIZENWSE,
    eSizeNESW  = SDL_SYSTEM_CURSOR_SIZENESW,
    eSizeWE    = SDL_SYSTEM_CURSOR_SIZEWE,
    eSizeNS    = SDL_SYSTEM_CURSOR_SIZENS,
    eSizeAll   = SDL_SYSTEM_CURSOR_SIZEALL,
    eNo        = SDL_SYSTEM_CURSOR_NO,
    eHand      = SDL_SYSTEM_CURSOR_HAND

};



class Application final : private internal::Singleton<Application>
{
private:
    std::unordered_map<Uint32, const EventDispatcher&> mEventDispatchers;
    std::unique_ptr<SDL_Cursor, decltype(&SDL_FreeCursor)> mCursor;

public:
    using Singleton<Application>::Instance;

    explicit Application(const SubSystem& aFlags) : mCursor(nullptr, &SDL_FreeCursor)
    {
        if (const auto lRet = SDL_Init(static_cast<Uint32>(aFlags)); lRet != 0)
            throw std::runtime_error("Failed to initialize SDL subsystems.");
    }

    ~Application()
    {
        SDL_Quit();
    }

    void Bind(const Uint32 aWindowId, const EventDispatcher& aWindow);
    void Run() const;

    void SetCursor(const CursorType aCursorType)
    {
        using Underlying = std::underlying_type_t<CursorType>;

        mCursor.reset(SDL_CreateSystemCursor(static_cast<SDL_SystemCursor>(static_cast<Underlying>(aCursorType))));

        SDL_SetCursor(mCursor.get());
    }

    [[maybe_unused]] static bool ShowCursor()
    {
        return SDL_ENABLE == SDL_ShowCursor(SDL_ENABLE);
    }

    [[maybe_unused]] static bool HideCursor()
    {
        return SDL_DISABLE == SDL_ShowCursor(SDL_DISABLE);
    }

    [[maybe_unused]] static bool IsCursorShown()
    {
        return SDL_ENABLE == SDL_ShowCursor(SDL_QUERY);
    }

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
