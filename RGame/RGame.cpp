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
#include "Systems/TimeEventManagerSystem.hpp"
#include "Systems/UISystem.hpp"
#include "Systems/HeightDependendLayerSystem.hpp"
#include <cassert>
#include <iostream>

///
/// Text pointer in an Button with color change for N, H, C
/// Scene Change on Click
/// Volume Change on Click
/// Map refactor
///


class App : public Scene
{
private:
    std::shared_ptr<Font> font = std::make_shared<Font>("Font/munro.ttf");
    
    entt::entity& makeNPC(const Position& pos, const std::shared_ptr<Texture>& t_Player,const float& textureX,const float& textureY, const std::string& path, const std::vector<Position>& poses, const entt::entity& player)
    {
        entt::entity NPCEntity = m_registry.create();
        Position& playerPosition = m_registry.emplace<Position>(NPCEntity, poses.at(0));

        Sprite npcSprite;
        {
            npcSprite.texture = t_Player;
            npcSprite.useTextureRect = true;
            npcSprite.textureRect = { textureX, textureY, 48.f, 48.f };
            npcSprite.sizeX = 48 * 2;
            npcSprite.sizeY = 48 * 2;
        }
        m_registry.emplace<Sprite>(NPCEntity, npcSprite);

        Animator animator;
        animator.anims.push_back(Animation{ "Idle",
            {
            Frame{{textureX,textureY,48,48}, 2.2f},
            Frame{{textureX + 1536,textureY,48,48}, 0.4f},
            } });
        animator.ToPlay = "Idle";

        m_registry.emplace<Animator>(NPCEntity, animator);

        NPC npc;
        LoadNPCFromJSON(path, npc);
        npc.positions = poses;

        m_registry.emplace<NPC>(NPCEntity, npc);

        HeightDependendLayer hdl;
        hdl.playerPos = &m_registry.get<Position>(player);
        hdl.playerSprite = &m_registry.get<Sprite>(player);
        m_registry.emplace<HeightDependendLayer>(NPCEntity, hdl);

        return NPCEntity;
    }


public:
    App(Game* game) : Scene(game)
    {
    }

    virtual void Init()
    {
        addSystem<RendererSystem>();
        addSystem<AnimatorSystem>();
        addSystem<HeightDependendLayerSystem>();
       

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
        PlayerInteractSystem* pis = addSystem<PlayerInteractSystem>(Player, ts, font);
        pms->setInteract(pis);

        addSystem<TimeEventManagerSystem>(pis);

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


        float m = 24 * 2; // tile size * scale
        entt::entity May = makeNPC({ 1200, 1850 }, t_Player, 0, 48 * 3, "DD/May.json", { 
            {m * 29, m * 40},
            {m * 40, m * 3}, 
            {m * 9, m * 3},
            {m * 25, m * 3} }, Player);

        entt::entity Carl = makeNPC({ 1200, 1850 }, t_Player, 3 * 48, 48 * 3, "DD/Carl.json", {
           {m * 9, m * 34},
           {m * 6, m * 24},
           {m * 38, m * 34},
           {m * 38, m * 34} }, Player);

        entt::entity msP = makeNPC({ 1200, 1850 }, t_Player, 2 * 48, 48 * 3, "DD/Ms. Pearce.json", {
           {m * 28, m * 21},
           {m * 999, m * 999},
           {m * 999, m * 999},
           {m * 45, m * 3} }, Player);

        entt::entity Ted = makeNPC({ 1200, 1850 }, t_Player, 1 * 48, 48 * 3, "DD/Ted.json", {
           {m * 26, m * 3},
           {m * 37, m * 28},
           {m * 33, m * 41 - 18},
           {m * 16, m * 41 - 18} }, Player);

        spdlog::info("Scene got init");
    }
       
};

class CreditsScene : public Scene
{
private:
    std::shared_ptr<Font> m_font = std::make_shared<Font>("Font/munro.ttf");
public:
    CreditsScene(Game* game) : Scene(game)
    {
    }

    virtual void Init()
    {
        spdlog::error("TO IMPLEMENT");
        m_game->StopGame();
    }
};

class Menu : public Scene
{
private:
    std::shared_ptr<Font> m_font = std::make_shared<Font>("Font/munro.ttf");
public:
    Menu(Game* game) : Scene(game)
    {
    }

    virtual void Init()
    {
        addSystem<RendererSystem>();
        addSystem<UISystem>(m_game);
        addSystem<AnimatorSystem>();

        std::shared_ptr<Texture> t_bg = CreateTexture("GPX/BG.png");
        std::shared_ptr<Texture> t_button = CreateTexture("GPX/buttons.png");
        std::shared_ptr<Texture> t_buttonSmall = CreateTexture("GPX/button_small.png");


        entt::entity BG = m_registry.create();

        ScreenPosition& BgPos = m_registry.emplace<ScreenPosition>(BG, ScreenPosition{ 0,0 });

        Sprite bgSprite;
        {
            bgSprite.texture = t_bg;
            bgSprite.sizeX = 1080;
            bgSprite.sizeY = 720;
            bgSprite.layerOrder = 2;
        }
        m_registry.emplace<Sprite>(BG, bgSprite);

        float FSV = 100;

        // Play Button
        {
            entt::entity Play = m_registry.create();
            entt::entity TText = m_registry.create();

            ScreenPosition& playPos = m_registry.emplace<ScreenPosition>(Play, ScreenPosition{ 444-400,260 - FSV });
            ScreenPosition& textPos = m_registry.emplace<ScreenPosition>(TText, ScreenPosition{ 502-400, 283 - FSV });

            Sprite playSprite;
            {
                playSprite.texture = t_button;
                playSprite.useTextureRect = true;
                playSprite.textureRect = { 0,0,32,32 };
                playSprite.sizeX = 32 * 6;
                playSprite.sizeY = 16 * 6;
                playSprite.layerOrder = 3;
            }
            m_registry.emplace<Sprite>(Play, playSprite);

            TextureButton tbPlay;
            tbPlay.ClickRect = { 0,0,32,32 };
            tbPlay.HoverRect = { 32,0,32,32 };
            m_registry.emplace<TextureButton>(Play, tbPlay);

            
            m_registry.emplace<ChangeSceneComponent>(Play, ChangeSceneComponent{new App(m_game)});

            Text t;
            t.color = { 235, 235, 230, 255 };
            t.content = "Play";
            t.font = m_font;
            t.xSize = 200;
            t.ySize = 200;
            t.fontSize = 48;

            m_registry.emplace<Text>(TText, t);
        }

        // Credits Button
        {
            entt::entity Credits = m_registry.create();
            entt::entity TText = m_registry.create();

            ScreenPosition& creditsPos = m_registry.emplace<ScreenPosition>(Credits, ScreenPosition{ 444-400,380 - FSV });
            ScreenPosition& textPos = m_registry.emplace<ScreenPosition>(TText, ScreenPosition{ 475-400, 403 - FSV });

            Sprite creditsSprite;
            {
                creditsSprite.texture = t_button;
                creditsSprite.useTextureRect = true;
                creditsSprite.textureRect = { 0,0,32,32 };
                creditsSprite.sizeX = 32 * 6;
                creditsSprite.sizeY = 16 * 6;
                creditsSprite.layerOrder = 3;
            }
            m_registry.emplace<Sprite>(Credits, creditsSprite);

            TextureButton tbCredits;
            tbCredits.ClickRect = { 0,0,32,32 };
            tbCredits.HoverRect = { 32,0,32,32 };
            m_registry.emplace<TextureButton>(Credits, tbCredits);
            m_registry.emplace<ChangeSceneComponent>(Credits, ChangeSceneComponent{ new CreditsScene(m_game) });


            Text t;
            t.color = { 235, 235, 230, 255 };
            t.content = "Credits";
            t.font = m_font;
            t.xSize = 200;
            t.ySize = 200;
            t.fontSize = 48;

            m_registry.emplace<Text>(TText, t);
        }
    
        // Quit Button
        {
            entt::entity Quit = m_registry.create();
            entt::entity TText = m_registry.create();

            ScreenPosition& quitPos = m_registry.emplace<ScreenPosition>(Quit, ScreenPosition{ 444-400,500 - FSV });
            ScreenPosition& textPos = m_registry.emplace<ScreenPosition>(TText, ScreenPosition{ 507-400, 523 - FSV });

            Sprite quitSprite;
            {
                quitSprite.texture = t_button;
                quitSprite.useTextureRect = true;
                quitSprite.textureRect = { 0,0,32,32 };
                quitSprite.sizeX = 32 * 6;
                quitSprite.sizeY = 16 * 6;
                quitSprite.layerOrder = 3;
            }
            m_registry.emplace<Sprite>(Quit, quitSprite);

            TextureButton tbQuit;
            tbQuit.ClickRect = { 0,0,32,32 };
            tbQuit.HoverRect = { 32,0,32,32 };
            m_registry.emplace<TextureButton>(Quit, tbQuit);

            m_registry.emplace<QuitGameEffector>(Quit, QuitGameEffector{false});

            Text t;
            t.color = { 235, 235, 230, 255 };
            t.content = "Quit";
            t.font = m_font;
            t.xSize = 200;
            t.ySize = 200;
            t.fontSize = 48;

            m_registry.emplace<Text>(TText, t);
        }


        // Volume Part
        {
            entt::entity Up = m_registry.create();
            entt::entity txtUp = m_registry.create();
            entt::entity Down = m_registry.create();
            entt::entity txtDown = m_registry.create();
            entt::entity Counter = m_registry.create();
            entt::entity Descriptor = m_registry.create();

            float TFV = -60;

            ScreenPosition& UpPos = m_registry.emplace<ScreenPosition>(Up, ScreenPosition{ 10,560 - TFV });
            ScreenPosition& TxtUpPos = m_registry.emplace<ScreenPosition>(txtUp, ScreenPosition{ 30,570 - TFV });
            ScreenPosition& DownPos = m_registry.emplace<ScreenPosition>(Down, ScreenPosition{ 200,560 - TFV });
            ScreenPosition& TxtDownPos = m_registry.emplace<ScreenPosition>(txtDown, ScreenPosition{ 223,565 - TFV });
            ScreenPosition& CounterPos = m_registry.emplace<ScreenPosition>(Counter, ScreenPosition{ 105,560 - TFV });
            ScreenPosition& DescriptorPos = m_registry.emplace<ScreenPosition>(Descriptor, ScreenPosition{ 65,500 - TFV });


            Sprite UpSprite;
            {
                UpSprite.texture = t_buttonSmall;
                UpSprite.useTextureRect = true;
                UpSprite.textureRect = { 0,0,16,16 };
                UpSprite.sizeX = 16 * 4;
                UpSprite.sizeY = 16 * 4;
                UpSprite.layerOrder = 3;
            }
            m_registry.emplace<Sprite>(Up, UpSprite);

            Sprite DowmSprite;
            {
                DowmSprite.texture = t_buttonSmall;
                DowmSprite.useTextureRect = true;
                DowmSprite.textureRect = { 0,0,16,16 };
                DowmSprite.sizeX = 16 * 4;
                DowmSprite.sizeY = 16 * 4;
                DowmSprite.layerOrder = 3;
            }
            m_registry.emplace<Sprite>(Down, DowmSprite);


            TextureButton tbUp;
            tbUp.ClickRect = { 0,0,16,16 };
            tbUp.HoverRect = { 16,0,16,16 };
            m_registry.emplace<TextureButton>(Up, tbUp);

            TextureButton tbDown;
            tbDown.ClickRect = { 0,0,16,16 };
            tbDown.HoverRect = { 16,0,16,16 };
            m_registry.emplace<TextureButton>(Down, tbDown);


            Text tUp;
            {
                tUp.color = { 235, 235, 230, 255 };
                tUp.content = "+";
                tUp.font = m_font;
                tUp.xSize = 200;
                tUp.ySize = 200;
                tUp.fontSize = 48;
            }
            m_registry.emplace<Text>(txtUp, tUp);

            Text tDown;
            {
                tDown.color = { 235, 235, 230, 255 };
                tDown.content = "-";
                tDown.font = m_font;
                tDown.xSize = 200;
                tDown.ySize = 200;
                tDown.fontSize = 48;
            }
            m_registry.emplace<Text>(txtDown, tDown);

            Text tCounter;
            {
                tCounter.color = { 235, 235, 230, 255 };
                tCounter.content = "100";
                tCounter.font = m_font;
                tCounter.xSize = 200;
                tCounter.ySize = 200;
                tCounter.fontSize = 48;
            }
            m_registry.emplace<Text>(Counter, tCounter);

            Text tDescriptor;
            {
                tDescriptor.color = { 235, 235, 230, 255 };
                tDescriptor.content = "Volume";
                tDescriptor.font = m_font;
                tDescriptor.xSize = 200;
                tDescriptor.ySize = 200;
                tDescriptor.fontSize = 48;
            }
            m_registry.emplace<Text>(Descriptor, tDescriptor);


        }
    
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
    //App* mainScene = new App(game);
    Menu* mainScene = new Menu(game);

    game->StartGame(mainScene);
    
    delete game;
    KitsuEngineClean();
    
    return 0;
}
