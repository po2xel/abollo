#ifndef __ABOLLO_WINDOW_EVENT_WINDOW_H__
#define __ABOLLO_WINDOW_EVENT_WINDOW_H__



#include "EventSlot.h"
#include "Window.h"



namespace abollo
{


template <auto... Es>
class EventWindow final : public Event<Es...>, public Window
{
private:
    using EventBase = Event<Es...>;

public:
    using Window::Window;
    using EventBase::On;

    virtual ~EventWindow() override = default;

    virtual void OnWindowEvent(const SDL_WindowEvent& aEvent) const override;

    virtual void OnMouseButtonDownEvent(const SDL_MouseButtonEvent& aEvent) const override;
    virtual void OnMouseButtonUpEvent(const SDL_MouseButtonEvent& aEvent) const override;
    virtual void OnMouseMotionEvent(const SDL_MouseMotionEvent& aEvent) const override;
    virtual void OnMouseWheelEvent(const SDL_MouseWheelEvent& aEvent) const override;

    virtual void OnKeyDownEvent(const SDL_KeyboardEvent& aEvent) const override;
    virtual void OnKeyUpEvent(const SDL_KeyboardEvent& aEvent) const override;
};


template <auto... Es>
void EventWindow<Es...>::OnWindowEvent(const SDL_WindowEvent& aEvent) const
{
    switch (static_cast<WindowEvent>(aEvent.event))
    {
    case WindowEvent::eShown:
        EventBase::template Signal<WindowEvent::eShown>();
        break;

    case WindowEvent::eHidden:
        EventBase::template Signal<WindowEvent::eHidden>();
        break;

    case WindowEvent::eExposed:
        EventBase::template Signal<WindowEvent::eExposed>();
        break;

    case WindowEvent::eMoved:
        EventBase::template Signal<WindowEvent::eMoved>(aEvent.data1, aEvent.data2);
        break;

    case WindowEvent::eResized:
        EventBase::template Signal<WindowEvent::eResized>(aEvent.data1, aEvent.data2);
        break;

    case WindowEvent::eSizeChanged:
        EventBase::template Signal<WindowEvent::eSizeChanged>();
        break;

    case WindowEvent::eMinimized:
        EventBase::template Signal<WindowEvent::eMinimized>();
        break;

    case WindowEvent::eMaximized:
        EventBase::template Signal<WindowEvent::eMaximized>();
        break;

    case WindowEvent::eRestored:
        EventBase::template Signal<WindowEvent::eRestored>();
        break;

    case WindowEvent::eEnter:
        EventBase::template Signal<WindowEvent::eEnter>();
        break;

    case WindowEvent::eLeave:
        EventBase::template Signal<WindowEvent::eLeave>();
        break;

    case WindowEvent::eFocusGained:
        EventBase::template Signal<WindowEvent::eFocusGained>();
        break;

    case WindowEvent::eFocusLost:
        EventBase::template Signal<WindowEvent::eFocusLost>();
        break;

    case WindowEvent::eClose:
        EventBase::template Signal<WindowEvent::eClose>();
        break;

    case WindowEvent::eTakeFocus:
        EventBase::template Signal<WindowEvent::eTakeFocus>();
        break;

    case WindowEvent::eHitTest:
        EventBase::template Signal<WindowEvent::eHitTest>();
        break;
    }
}


template <auto... Es>
void EventWindow<Es...>::OnMouseButtonDownEvent(const SDL_MouseButtonEvent& aEvent) const
{
    switch (static_cast<MouseButton>(aEvent.button))
    {
    case MouseButton::eLeft:
        EventBase::template Signal<MouseEvent::eLButtonDown>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eMiddle:
        EventBase::template Signal<MouseEvent::eMButtonDown>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eRight:
        EventBase::template Signal<MouseEvent::eRButtonDown>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eX1:
        EventBase::template Signal<MouseEvent::eX1ButtonDown>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eX2:
        EventBase::template Signal<MouseEvent::eX2ButtonDown>(aEvent.x, aEvent.y);
        break;
    }
}


template <auto... Es>
void EventWindow<Es...>::OnMouseButtonUpEvent(const SDL_MouseButtonEvent& aEvent) const
{
    switch (static_cast<MouseButton>(aEvent.button))
    {
    case MouseButton::eLeft:
        EventBase::template Signal<MouseEvent::eLButtonUp>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eMiddle:
        EventBase::template Signal<MouseEvent::eMButtonUp>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eRight:
        EventBase::template Signal<MouseEvent::eRButtonUp>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eX1:
        EventBase::template Signal<MouseEvent::eX1ButtonUp>(aEvent.x, aEvent.y);
        break;

    case MouseButton::eX2:
        EventBase::template Signal<MouseEvent::eX2ButtonUp>(aEvent.x, aEvent.y);
        break;
    }
}


template <auto... Es>
void EventWindow<Es...>::OnMouseMotionEvent(const SDL_MouseMotionEvent& aEvent) const
{
    EventBase::template Signal<MouseEvent::eMotion>(aEvent.x, aEvent.y, aEvent.xrel, aEvent.yrel, aEvent.state);
}


template <auto... Es>
void EventWindow<Es...>::OnMouseWheelEvent(const SDL_MouseWheelEvent& aEvent) const
{
    EventBase::template Signal<MouseEvent::eWheel>(aEvent.x, aEvent.y);
}


template <auto... Es>
void EventWindow<Es...>::OnKeyDownEvent(const SDL_KeyboardEvent& aEvent) const
{
    EventBase::template Signal<KeyEvent::eDown>(static_cast<Key>(aEvent.keysym.sym), aEvent.keysym.mod);
}


template <auto... Es>
void EventWindow<Es...>::OnKeyUpEvent(const SDL_KeyboardEvent& aEvent) const
{
    EventBase::template Signal<KeyEvent::eUp>(static_cast<Key>(aEvent.keysym.sym), aEvent.keysym.mod);
}



}    // namespace abollo



#endif    // __ABOLLO_WINDOW_EVENT_WINDOW_H__
