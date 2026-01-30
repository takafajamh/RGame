#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>


#include <SDL3/SDL.h>
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
    
    std::shared_ptr<Font> font = std::make_shared<Font>("assets/Font/munro.ttf");
    

    void SetupHall(const std::shared_ptr<Texture>& t_manager, const std::shared_ptr<Texture> t_right)
    {
        std::shared_ptr<Texture> t_counter = CreateTexture("assets/GPX/Counter.png");
        std::shared_ptr<Texture> t_wall = CreateTexture("assets/GPX/Wall.png");
        std::shared_ptr<Texture> t_bg = CreateTexture("assets/GPX/bg.png");

        // BG, layer 2
        {
            entt::entity bg = m_registry.create();
            m_registry.emplace<Position>(bg, Position{0,0});
            Sprite s_bg;

            s_bg.layerOrder = 2;
            s_bg.texture = t_bg;
            s_bg.sizeX = 1920;
            s_bg.sizeY = 1080;
            m_registry.emplace<Sprite>(bg, s_bg);
        }

        // wall, layer 6
        {
            entt::entity wall = m_registry.create();
            m_registry.emplace<Position>(wall, Position{ 0,1080 - 600 });
            Sprite s_wall;

            s_wall.layerOrder = 6;
            s_wall.texture = t_wall;
            s_wall.sizeX = 400;
            s_wall.sizeY = 600;
            m_registry.emplace<Sprite>(wall, s_wall);
        }

        // counter, layer 6
        {
            entt::entity counter = m_registry.create();
            m_registry.emplace<Position>(counter, Position{ 400, 1080 - 400 });
            Sprite s_counter;

            s_counter.layerOrder = 6;
            s_counter.texture = t_counter;
            s_counter.sizeX = 1000;
            s_counter.sizeY = 400;
            m_registry.emplace<Sprite>(counter, s_counter);
        }

        // manager, layer 4
        {
            entt::entity manager = m_registry.create();
            m_registry.emplace<Position>(manager, Position{ 450, 1080 - (3508 / 4) - 100});
            Sprite s_manager;

            s_manager.layerOrder = 4;
            s_manager.texture = t_manager;
            s_manager.sizeX = 2480 / 4; // 2480
            s_manager.sizeY = 3508 / 4;  // 3508
            m_registry.emplace<Sprite>(manager, s_manager);
        }

        // right, layer 7
        {
            entt::entity right = m_registry.create();
            m_registry.emplace<Position>(right, Position{ 1800, 1080 - 280 - 30 });
            
            Sprite s_right;
            s_right.layerOrder = 7;
            s_right.texture = t_right;
            s_right.sizeX = 40;
            s_right.sizeY = 280;  
            s_right.useTextureRect = true;
            s_right.textureRect = { 0, 0, 40, 280 };
            m_registry.emplace<Sprite>(right, s_right);

            TextureButton tb;
            tb.ClickRect = { 0,0,40,280 };
            tb.HoverRect = { 40,0,40,280 };
            m_registry.emplace<TextureButton>(right, tb);

            MoveCameraEffector mce;
            mce.dx = 1920;
            mce.dy = 0;
            m_registry.emplace<MoveCameraEffector>(right, mce);


        }


    }

public:
    App(Game* game) : Scene(game)
    {
    }

    virtual void Init()
    {
        RendererSystem* rs = addSystem<RendererSystem>();
        rs->camXPos = 1920 / 2;
        rs->camYPos = 1080 / 2;
        addSystem<AnimatorSystem>();
        addSystem<UISystem>();
        spdlog::info("Scene got init");


        std::shared_ptr<Texture> t_manager = CreateTexture("assets/GPX/Miss manager.png");
        std::shared_ptr<Texture> t_right = CreateTexture("assets/GPX/right.png");
    
    
        SetupHall(t_manager, t_right);

        
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

    WindowParams wp;
    wp.Fullscreen = true;

    int res = KitsuEngineInit(1920, 1080, "OwO", &wp);
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


