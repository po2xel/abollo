#ifndef __ABOLLO_WINDOW_EVENT_SLOT_H__
#define __ABOLLO_WINDOW_EVENT_SLOT_H__



#include <type_traits>

#include <boost/signals2.hpp>



namespace abollo
{



using boost::signals2::connect_position;
using boost::signals2::connection;
using boost::signals2::signal;



enum class MouseButton : Uint8
{
    eLeft   = SDL_BUTTON_LEFT,
    eMiddle = SDL_BUTTON_MIDDLE,
    eRight  = SDL_BUTTON_RIGHT,
    eX1     = SDL_BUTTON_X1,
    eX2     = SDL_BUTTON_X2
};


enum class MouseMask : Uint8
{
    eLeft   = SDL_BUTTON_LMASK,
    eMiddle = SDL_BUTTON_MMASK,
    eRight  = SDL_BUTTON_RMASK,
    eX1     = SDL_BUTTON_X1MASK,
    eX2     = SDL_BUTTON_X2MASK
};


constexpr auto operator&(const Uint32 aMask, const MouseMask& aLhs)
{
    using Underlying = std::underlying_type_t<MouseMask>;

    return static_cast<MouseMask>(static_cast<Underlying>(aLhs) & aMask);
}

constexpr auto operator&(const MouseMask& aLhs, const Uint32 aMask)
{
    return aMask & aLhs;
}


enum class KeyModifier : Uint16
{
    eNone   = KMOD_NONE,
    eLShift = KMOD_LSHIFT,
    eRShift = KMOD_RSHIFT,
    eLCtrl  = KMOD_LCTRL,
    eRCtrl  = KMOD_RCTRL,
    eLAlt   = KMOD_LALT,
    eRAlt   = KMOD_RALT,
    eLGui   = KMOD_LGUI,
    eRGui   = KMOD_RGUI,
    eNum    = KMOD_NUM,
    eCaps   = KMOD_CAPS,
    eMode   = KMOD_MODE,

    eCtrl = KMOD_CTRL,
    eShit = KMOD_SHIFT,
    eAlt  = KMOD_ALT,
    eGui  = KMOD_GUI
};


enum class Key : Uint32
{
    eUnknown = SDLK_UNKNOWN,

    eReturn     = SDLK_RETURN,
    eEsc        = SDLK_ESCAPE,
    eBackSpace  = SDLK_BACKSPACE,
    eTab        = SDLK_TAB,
    eSpace      = SDLK_SPACE,
    eExclaim    = SDLK_EXCLAIM,
    eQuoteDbl   = SDLK_QUOTEDBL,
    eHash       = SDLK_HASH,
    ePercent    = SDLK_PERCENT,
    eDollar     = SDLK_DOLLAR,
    eAmpersand  = SDLK_AMPERSAND,
    eQuote      = SDLK_QUOTE,
    eLParen     = SDLK_LEFTPAREN,
    eRParen     = SDLK_RIGHTPAREN,
    eAsterisk   = SDLK_ASTERISK,
    ePlus       = SDLK_PLUS,
    eComma      = SDLK_COMMA,
    eMinus      = SDLK_MINUS,
    ePeriod     = SDLK_PERIOD,
    eSlash      = SDLK_SLASH,
    eColon      = SDLK_COLON,
    eSemiColon  = SDLK_SEMICOLON,
    eLess       = SDLK_LESS,
    eEqual      = SDLK_EQUALS,
    eGreater    = SDLK_GREATER,
    eQuestion   = SDLK_QUESTION,
    eAt         = SDLK_AT,
    eLBracket   = SDLK_LEFTBRACKET,
    eBackSlash  = SDLK_BACKSLASH,
    eRBracket   = SDLK_RIGHTBRACKET,
    eCaret      = SDLK_CARET,
    eUnderscore = SDLK_UNDERSCORE,
    eBackQuote  = SDLK_BACKQUOTE,

    e0 = SDLK_0,
    e1 = SDLK_1,
    e2 = SDLK_2,
    e3 = SDLK_3,
    e4 = SDLK_4,
    e5 = SDLK_5,
    e6 = SDLK_6,
    e7 = SDLK_7,
    e8 = SDLK_8,
    e9 = SDLK_9,

    eA = SDLK_a,
    eB = SDLK_b,
    eC = SDLK_c,
    eD = SDLK_d,
    eF = SDLK_f,
    eG = SDLK_g,
    eH = SDLK_h,
    eI = SDLK_i,
    eJ = SDLK_j,
    eK = SDLK_k,
    eL = SDLK_l,
    eM = SDLK_m,
    eN = SDLK_n,
    eO = SDLK_o,
    eP = SDLK_p,
    eQ = SDLK_q,
    eR = SDLK_r,
    eS = SDLK_s,
    eT = SDLK_t,
    eU = SDLK_u,
    eV = SDLK_v,
    eW = SDLK_w,
    eX = SDLK_x,
    eY = SDLK_y,
    eZ = SDLK_z,

    eCaps = SDLK_CAPSLOCK,

    eF1  = SDLK_F1,
    eF2  = SDLK_F2,
    eF3  = SDLK_F3,
    eF4  = SDLK_F4,
    eF5  = SDLK_F5,
    eF6  = SDLK_F6,
    eF7  = SDLK_F7,
    eF8  = SDLK_F8,
    eF9  = SDLK_F9,
    eF10 = SDLK_F10,
    eF11 = SDLK_F11,
    eF12 = SDLK_F12,

    ePrintScreen = SDLK_PRINTSCREEN,
    eScrollLock  = SDLK_SCROLLLOCK,
    ePause       = SDLK_PAUSE,
    eInsert      = SDLK_INSERT,
    eHome        = SDLK_HOME,
    ePageUp      = SDLK_PAGEUP,
    eDelete      = SDLK_DELETE,
    eEnd         = SDLK_END,
    ePageDown    = SDLK_PAGEDOWN,
    eRight       = SDLK_RIGHT,
    eLeft        = SDLK_LEFT,
    eDown        = SDLK_DOWN,
    eUp          = SDLK_UP
};



enum class WindowEvent : Uint8
{
    eShown       = SDL_WINDOWEVENT_SHOWN,
    eHidden      = SDL_WINDOWEVENT_HIDDEN,
    eExposed     = SDL_WINDOWEVENT_EXPOSED,
    eMoved       = SDL_WINDOWEVENT_MOVED,
    eResized     = SDL_WINDOWEVENT_RESIZED,
    eSizeChanged = SDL_WINDOWEVENT_SIZE_CHANGED,
    eMinimized   = SDL_WINDOWEVENT_MINIMIZED,
    eMaximized   = SDL_WINDOWEVENT_MAXIMIZED,
    eRestored    = SDL_WINDOWEVENT_RESTORED,
    eEnter       = SDL_WINDOWEVENT_ENTER,
    eLeave       = SDL_WINDOWEVENT_LEAVE,
    eFocusGained = SDL_WINDOWEVENT_FOCUS_GAINED,
    eFocusLost   = SDL_WINDOWEVENT_FOCUS_LOST,
    eClose       = SDL_WINDOWEVENT_CLOSE,
    eTakeFocus   = SDL_WINDOWEVENT_TAKE_FOCUS,
    eHitTest     = SDL_WINDOWEVENT_HIT_TEST
};



enum class MouseEvent : Uint8
{
    eLButtonDown,
    eLButtonUp,

    eRButtonDown,
    eRButtonUp,

    eMButtonDown,
    eMButtonUp,

    eX1ButtonDown,
    eX1ButtonUp,

    eX2ButtonDown,
    eX2ButtonUp,

    eWheel,

    eMotion
};


enum class KeyEvent : Uint8
{
    eDown,
    eUp
};


namespace internal
{


template <typename T, T e>
class EventTrait
{
private:
    signal<void()> mSlot;
    connection mConnection;

public:
    template <typename Slot>
    void On(Slot&& aSlot, const connect_position aPos = connect_position::at_back)
    {
        mConnection = mSlot.connect(std::forward<Slot>(aSlot), aPos);
    }

    void Off() const
    {
        mConnection.disconnect();
    }

    template <typename... Args>
    void Signal(Args&&... aArgs) const
    {
        std::invoke(mSlot, std::forward<Args>(aArgs)...);
    }
};



template <>
class EventTrait<WindowEvent, WindowEvent::eMoved>
{
private:
    signal<void(const Sint32 aWidth, const Sint32 aHeight)> mSlot;
    connection mConnection;

public:
    template <typename Slot>
    void On(Slot&& aSlot, const connect_position aPos = connect_position::at_back)
    {
        mConnection = mSlot.connect(std::forward<Slot>(aSlot), aPos);
    }

    void Off() const
    {
        mConnection.disconnect();
    }

    template <typename... Args>
    void Signal(Args&&... aArgs) const
    {
        std::invoke(mSlot, std::forward<Args>(aArgs)...);
    }
};



template <>
class EventTrait<WindowEvent, WindowEvent::eResized>
{
private:
    signal<void(const Sint32 aWidth, const Sint32 aHeight)> mSlot;
    connection mConnection;

public:
    template <typename Slot>
    void On(Slot&& aSlot, const connect_position aPos = connect_position::at_back)
    {
        mConnection = mSlot.connect(std::forward<Slot>(aSlot), aPos);
    }

    void Off() const
    {
        mConnection.disconnect();
    }

    template <typename... Args>
    void Signal(Args&&... aArgs) const
    {
        std::invoke(mSlot, std::forward<Args>(aArgs)...);
    }
};



template <MouseEvent e>
class EventTrait<MouseEvent, e>
{
private:
    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mSlot;
    connection mConnection;

public:
    template <typename Slot>
    void On(Slot&& aSlot, const connect_position aPos = connect_position::at_back)
    {
        mConnection = mSlot.connect(std::forward<Slot>(aSlot), aPos);
    }

    void Off() const
    {
        mConnection.disconnect();
    }

    template <typename... Args>
    void Signal(Args&&... aArgs) const
    {
        std::invoke(mSlot, std::forward<Args>(aArgs)...);
    }
};



template <>
class EventTrait<MouseEvent, MouseEvent::eMotion>
{
private:
    signal<void(const Sint32 aPosX, const Sint32 aPosY, const Sint32 aPosRelX, const Sint32 aPosRelY, const Uint32 aMask)> mSlot;
    connection mConnection;

public:
    template <typename Slot>
    void On(Slot&& aSlot, const connect_position aPos = connect_position::at_back)
    {
        mConnection = mSlot.connect(std::forward<Slot>(aSlot), aPos);
    }

    void Off() const
    {
        mConnection.disconnect();
    }

    template <typename... Args>
    void Signal(Args&&... aArgs) const
    {
        std::invoke(mSlot, std::forward<Args>(aArgs)...);
    }
};



template <KeyEvent e>
class EventTrait<KeyEvent, e>
{
private:
    signal<void(const Key aKey, const Uint16 aModifier)> mSlot;
    connection mConnection;

public:
    template <typename Slot>
    void On(Slot&& aSlot, const connect_position aPos = connect_position::at_back)
    {
        mConnection = mSlot.connect(std::forward<Slot>(aSlot), aPos);
    }

    void Off() const
    {
        mConnection.disconnect();
    }

    template <typename... Args>
    void Signal(Args&&... aArgs) const
    {
        std::invoke(mSlot, std::forward<Args>(aArgs)...);
    }
};



}    // namespace internal



template <auto... Es>
class EventSlot : private internal::EventTrait<decltype(Es), Es>...
{
private:
    template <typename... Args>
    constexpr static void SwallowArgs(Args&&... /*aArgs*/)
    {
    }

public:
    template <auto... E, typename Slot>
    void On(Slot&& aSlot)
    {
        static_assert((std::is_base_of_v<internal::EventTrait<decltype(E), E>, EventSlot<Es...>> && ...), "Event is not specified.");

        (internal::EventTrait<decltype(E), E>::On(std::forward<Slot>(aSlot)), ...);
    }

    template <auto... E>
    void Off()
    {
        static_assert((std::is_base_of_v<internal::EventTrait<decltype(E), E>, EventSlot<Es...>> && ...), "Event is not specified.");

        (internal::EventTrait<decltype(E), E>::Off(), ...);
    }

    template <auto... E, typename... Args>
    constexpr void Signal(Args&&... aArgs) const
    {
        // static_assert((std::is_base_of_v<internal::EventTrait<decltype(E), E>, EventSlot<Es...>> && ...), "Event is not specified.");

        if constexpr ((std::is_base_of_v<internal::EventTrait<decltype(E), E>, EventSlot<Es...>> && ...))
            (internal::EventTrait<decltype(E), E>::Signal(std::forward<Args>(aArgs)...), ...);
        else
            SwallowArgs(std::forward<Args>(aArgs)...);
    }
};



}    // namespace abollo



#endif    // __ABOLLO_WINDOW_EVENT_SLOT_H__
