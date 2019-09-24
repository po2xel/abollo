#ifndef __ABOLLO_EVENT_DISPATCHER_H__
#define __ABOLLO_EVENT_DISPATCHER_H__
#include <SDL2/SDL.h>



namespace abollo
{



class EventDispatcher
{
public:
    virtual ~EventDispatcher() = default;

    virtual void OnWindowEvent(const SDL_WindowEvent& aEvent) const = 0;

    virtual void OnMouseButtonDownEvent(const SDL_MouseButtonEvent& aEvent) const = 0;
    virtual void OnMouseButtonUpEvent(const SDL_MouseButtonEvent& aEvent) const   = 0;
    virtual void OnMouseMotionEvent(const SDL_MouseMotionEvent& aEvent) const     = 0;
    virtual void OnMouseWheelEvent(const SDL_MouseWheelEvent& aEvent) const       = 0;

    virtual void OnKeyDownEvent(const SDL_KeyboardEvent& aEvent) const = 0;
    virtual void OnKeyUpEvent(const SDL_KeyboardEvent& aEvent) const   = 0;
};



}    // namespace abollo



#endif    // !__ABOLLO_EVENT_DISPATCHER_H__
