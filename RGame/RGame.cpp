#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_Image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "Components.hpp"

#include "Systems/RendererSystem.hpp"
#include "Systems/AnimatorSystem.hpp"
#include "Systems/RemoveAfterDelaySystem.hpp"
#include "Systems/UISystem.hpp"

#include <cassert>
#include <iostream>
#include <regex>
#include <algorithm>
#include <random>


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
        addSystem<RendererSystem>();
        addSystem<AnimatorSystem>();
        addSystem<UISystem>();
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

Game* game;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
    srand(time(NULL));
    int res = KitsuEngineInit(1600, 900, "OwO");
    if (res == -1)
    {
        return SDL_APP_FAILURE;
    }
    game = new Game();
    App* mainScene = new App(game);
    game->StartGame(mainScene);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    game->MainLoop();
    return SDL_APP_CONTINUE;

}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    bool quit = game->HandleEvents(event);
    if (quit)
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    delete game;
    KitsuEngineClean();
    SDL_Quit();
}


