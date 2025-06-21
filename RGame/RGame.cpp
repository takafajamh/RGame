#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_Image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "Components.hpp"

#include "Systems/RendererSystem.hpp"
#include "Systems/PlayerMovementSystem.hpp"
#include "Systems/PlayerInteract.hpp"
#include "Systems/TilemapSystem.hpp"
#include "Systems/AnimatorSystem.hpp"
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
        addSystem<AnimatorSystem>();

        TilemapSystem* ts = addSystem<TilemapSystem>();

        std::shared_ptr<Texture> t_Player = CreateTexture("GPX/characters-sheet.png");
        

        entt::entity Player = m_registry.create();
        Position& playerPosition = m_registry.emplace<Position>(Player, Position{1150,1900});
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

        PlayerMovementSystem* pms = addSystem<PlayerMovementSystem>(Player, ts);
        pms->setInteract(addSystem<PlayerInteractSystem>(Player, ts, font));

        float Dress = 0;

        Animator animator;
        animator.anims.push_back(Animation{ "Idle",
            {
            Frame{{Dress*48,0,48,48}, 2.2f},
            Frame{{(Dress * 48) + 1536,0,48,48}, 0.4f},
            }});
       
        animator.anims.push_back(Animation{ "Backward",
            {
            Frame{{Dress * 48,0,48,48}, 0.15f},
            Frame{{(Dress + 4) * 48,0,48,48}, 0.15f},
            Frame{{(Dress + 8) * 48,0,48,48}, 0.15f},
            Frame{{(Dress + 12) * 48,0,48,48}, 0.15f},
            } });
        animator.anims.push_back(Animation{ "Forward",
            {
            Frame{{(Dress + 16) * 48,0,48,48}, 0.15f},
            Frame{{(Dress + 20) * 48,0,48,48}, 0.15f},
            Frame{{(Dress + 24) * 48,0,48,48}, 0.15f},
            Frame{{(Dress + 28) * 48,0,48,48}, 0.15f},
            } });
        
        m_registry.emplace<Animator>(Player, animator);

        std::shared_ptr<Texture> t_Tilesheet = CreateTexture("GPX/tilesheet.png");
        ts->LoadMap(m_registry, "GPX/mapa.json", t_Tilesheet, Position{ 0,0 });


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
