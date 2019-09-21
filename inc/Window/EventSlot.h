#ifndef __ABOLLO_WINDOW_EVENT_SLOT_H__
#define __ABOLLO_WINDOW_EVENT_SLOT_H__



#include <type_traits>

#include <boost/signals2.hpp>



namespace abollo
{



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
    eF14 = SDLK_F14,
    eF15 = SDLK_F15,
    eF16 = SDLK_F16,
    eF17 = SDLK_F17,
    eF18 = SDLK_F18,
    eF19 = SDLK_F19,
    eF20 = SDLK_F20,
    eF21 = SDLK_F21,
    eF22 = SDLK_F22,
    eF23 = SDLK_F23,
    eF24 = SDLK_F24,

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



#define STATIC_EVENT_CONNECT_IF(E, EventCat, EventType, slot) \
    if constexpr (E == EventCat::e##EventType)                \
        m##EventType##Slot.connect(std::forward<Slot>(slot));

#define STATIC_EVENT_CONNECT_ELIF(E, EventCat, EventType, slot) else STATIC_EVENT_CONNECT_IF(E, EventCat, EventType, slot)


#define STATIC_EVENT_SIGNAL_IF(E, EventCat, EventType, A, args) \
    if constexpr (E == EventCat::e##EventType)                  \
        std::invoke(m##EventType##Slot, std::forward<A>(args)...);

#define STATIC_EVENT_SIGNAL_ELIF(E, EventCat, EventType, A, args) else STATIC_EVENT_SIGNAL_IF(E, EventCat, EventType, A, args)

#define STATIC_EVENT_ELSE_FALSE else static_assert(false, "Event type is not supproted.");



class MouseEventSlot
{
private:
    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mLButtonDownSlot;
    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mLButtonUpSlot;

    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mMButtonDownSlot;
    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mMButtonUpSlot;

    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mRButtonDownSlot;
    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mRButtonUpSlot;

    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mX1ButtonDownSlot;
    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mX1ButtonUpSlot;

    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mX2ButtonDownSlot;
    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mX2ButtonUpSlot;

    signal<void(const Sint32 aPosX, const Sint32 aPosY)> mWheelSlot;

    signal<void(const Sint32 aPosX, const Sint32 aPosY, const Sint32 aPosRelX, const Sint32 aPosRelY, const Uint32 aMask)> mMotionSlot;

public:
    template <MouseEvent E, typename Slot>
    constexpr void On(Slot&& aSlot)
    {
        STATIC_EVENT_CONNECT_IF(E, MouseEvent, LButtonDown, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, LButtonUp, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, MButtonDown, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, MButtonUp, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, RButtonDown, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, RButtonUp, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, X1ButtonDown, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, X1ButtonUp, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, X2ButtonDown, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, X2ButtonUp, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, Wheel, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, MouseEvent, Motion, aSlot)
        STATIC_EVENT_ELSE_FALSE
    }


    template <MouseEvent E, typename... Args>
    constexpr void Signal(Args&&... aArgs) const
    {
        STATIC_EVENT_SIGNAL_IF(E, MouseEvent, LButtonDown, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, LButtonUp, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, MButtonDown, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, MButtonUp, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, RButtonDown, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, RButtonUp, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, X1ButtonDown, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, X1ButtonUp, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, X2ButtonDown, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, X2ButtonUp, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, Wheel, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, MouseEvent, Motion, Args, aArgs)
        STATIC_EVENT_ELSE_FALSE
    }
};



class KeyEventSlot
{
private:
    signal<void(const Key aKey, const Uint16 aModifier)> mDownSlot;
    signal<void(const Key aKey, const Uint16 aModifier)> mUpSlot;

public:
    template <KeyEvent E, typename Slot>
    constexpr void On(Slot&& aSlot)
    {
        STATIC_EVENT_CONNECT_IF(E, KeyEvent, Down, aSlot)
        STATIC_EVENT_CONNECT_ELIF(E, KeyEvent, Up, aSlot)
        STATIC_EVENT_ELSE_FALSE
    }

    template <KeyEvent E, typename... Args>
    constexpr void Signal(Args&&... aArgs) const
    {
        STATIC_EVENT_SIGNAL_IF(E, KeyEvent, Down, Args, aArgs)
        STATIC_EVENT_SIGNAL_ELIF(E, KeyEvent, Up, Args, aArgs)
        STATIC_EVENT_ELSE_FALSE
    }
};



}    // namespace abollo



#endif    // __ABOLLO_WINDOW_EVENT_SLOT_H__
