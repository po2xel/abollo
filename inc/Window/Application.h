#ifndef __ABOLLO_WINDOW_APPLICATION_H__
#define __ABOLLO_WINDOW_APPLICATION_H__



#include <stdexcept>
#include <unordered_map>

#include "Singleton.h"
#include "Typedef.h"



namespace abollo
{



class Window;


class Application final : private internal::Singleton<Application>
{
private:
    std::unordered_map<Uint32, const Window&> mWindows;

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
};



}    // namespace abollo



#endif    // !__ABOLLO_WINDOW_APPLICATION_H__
