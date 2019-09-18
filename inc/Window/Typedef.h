#ifndef __ABOLLO_WINDOW_TYPEDEF_H__
#define __ABOLLO_WINDOW_TYPEDEF_H__



#include <type_traits>

#include <SDL2/SDL.h>



namespace abollo
{



using WindowHandle = SDL_Window;

using WindowEvent      = SDL_WindowEvent;
using KeyboardEvent    = SDL_KeyboardEvent;
using MouseMotionEvent = SDL_MouseMotionEvent;
using MouseButtonEvent = SDL_MouseButtonEvent;
using MouseWheelEvent  = SDL_MouseWheelEvent;



enum class Event : Uint32
{
    eWindowEvent     = SDL_WINDOWEVENT,
    eMouseMotion     = SDL_MOUSEMOTION,
    eMouseButtonDown = SDL_MOUSEBUTTONDOWN,
    eMouseButtonUp   = SDL_MOUSEBUTTONUP,
    eMouseWheel      = SDL_MOUSEWHEEL,
    eKeyDown         = SDL_KEYDOWN,
    eKeyUp           = SDL_KEYUP
};



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



}    // namespace abollo



#endif    // !__ABOLLO_WINDOW_TYPEDEF_H__
