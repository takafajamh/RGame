#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_Image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "Components.hpp"

#include <cassert>
#include <iostream>

class App : public Scene
{
private:
    std::shared_ptr<Font> font = std::make_shared<Font>("Font/munro.ttf");
    
public:
    App(Game* game) : Scene(game)
    {
    }

    virtual void Init()
    {
        //entt::entity CheckBox = m_registry.create();
        //m_registry.emplace<ScreenPosition>(CheckBox, ScreenPosition{ 10.f, 10.f });

        entt::entity Player = m_registry.create();
        m_registry.emplace<Position>(Player, Position{300,300});




        spdlog::info("Scene got init");
    }
};

std::pair<int,int> GetScreenSize()
{
    SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
    if (primaryDisplay == 0)
    {
        spdlog::error("Failed to get primary display: {}", SDL_GetError());
    }
    else
    {
        SDL_Rect bounds;
        if (!SDL_GetDisplayBounds(primaryDisplay, &bounds))
        {
            spdlog::error("SDL_GetDisplayBounds failed: {}", SDL_GetError());
        }
        else
        {
            int screenWidth = bounds.w;
            int screenHeight = bounds.h;
            spdlog::info("Primary screen size: {} x {}", screenWidth, screenHeight);
            return std::pair<int, int>(screenWidth, screenHeight);
        }
    }

    return { 0,0 };
}

int main(int argc, char** argv)
{
    KitsuEngineInit(1080, 720, "SPY");

    Game* game = new Game();
    App* mainScene = new App(game);

    game->StartGame(mainScene);
    
    delete game;
    KitsuEngineClean();
    
    return 0;
}
