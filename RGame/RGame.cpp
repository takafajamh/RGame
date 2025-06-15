#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_Image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "Components.hpp"

#include "Systems/RendererSystem.hpp"
#include "Systems/PlayerMovementSystem.hpp"

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
        addSystem<RendererSystem>();


        std::shared_ptr<Texture> t_Player = CreateTexture("GPX/characters-sheet.png");
        

        entt::entity Player = m_registry.create();
        Position& playerPosition = m_registry.emplace<Position>(Player, Position{0,0});
        xCamPos = &playerPosition.x;
        yCamPos = &playerPosition.y;

        Sprite playerSprite;
        {
            playerSprite.texture = t_Player;
            playerSprite.useTextureRect = true;
            playerSprite.textureRect = { 0.f, 0.f, 48.f, 48.f };
            playerSprite.sizeX = 48 * 2;
            playerSprite.sizeY = 48 * 2;
        }
        m_registry.emplace<Sprite>(Player, playerSprite);

        addSystem<PlayerMovementSystem>(Player);


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
