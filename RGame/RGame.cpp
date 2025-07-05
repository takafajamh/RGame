#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_Image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "Components.hpp"

#include "Systems/RendererSystem.hpp"
#include "Systems/AnimatorSystem.hpp"
#include "Systems/MoverSystem.hpp"
#include "Systems/NoteSpawner.hpp"
#include "Systems/NoteClickSystem.hpp"
#include "Systems/RemoveAfterDelaySystem.hpp"
#include "Systems/NoteRecorder.hpp"
#include "Systems/InputTRectChangeSystem.hpp"
#include "Systems/UISystem.hpp"

#include <cassert>
#include <iostream>

///
/// Map refactor
///

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
        addSystem<MoverSystem>();
        addSystem<RemoveAfterDelaySystem>();
        NoteClickSystem* ncs = addSystem<NoteClickSystem>();
        addSystem<InputTRectChangeSystem>();
        addSystem<UISystem>();

        std::shared_ptr<Texture> t_Arrows = CreateTexture("GPX/Arrows.png");
        std::shared_ptr<Texture> t_char1 = CreateTexture("GPX/char1.png");
        std::shared_ptr<Texture> t_bg = CreateTexture("GPX/bg.png");

        // Arrows down
        {
            entt::entity LArrow = m_registry.create();
            entt::entity RArrow = m_registry.create();
            entt::entity UArrow = m_registry.create();
            entt::entity DArrow = m_registry.create();

            float m = 100;
            float sPos = 660;

            m_registry.emplace<ScreenPosition>(LArrow, ScreenPosition{ sPos, 600 });
            m_registry.emplace<ScreenPosition>(UArrow, ScreenPosition{ sPos + m, 600 });
            m_registry.emplace<ScreenPosition>(DArrow, ScreenPosition{ sPos + 2 * m, 600 });
            m_registry.emplace<ScreenPosition>(RArrow, ScreenPosition{ sPos + 3 * m, 600 });

            Sprite SLA;
            SLA.sizeX = 16 * 4;
            SLA.sizeY = 16 * 4;
            SLA.texture = t_Arrows;
            SLA.useTextureRect = true;
            SLA.textureRect = { 16 * 0, 16 * 0, 16, 16 };
            m_registry.emplace<Sprite>(LArrow, SLA);
    
            Sprite SRA = SLA;
            SRA.textureRect = { 16 * 0, 16 * 1, 16, 16 };
            m_registry.emplace<Sprite>(RArrow, SRA);

            Sprite SUA = SLA;
            SUA.textureRect = { 16 * 0, 16 * 2, 16, 16 };
            m_registry.emplace<Sprite>(UArrow, SUA);

            Sprite SDA = SLA;
            SDA.textureRect = { 16 * 0, 16 * 3, 16, 16 };
            m_registry.emplace<Sprite>(DArrow, SDA);


            m_registry.emplace<NoteReceiver>(LArrow, NoteReceiver{ { 16 * 0, 16 * 0, 16, 16 }, { 16 * 1, 16 * 0, 16, 16 }, SDL_SCANCODE_LEFT , 0 });
            m_registry.emplace<NoteReceiver>(RArrow, NoteReceiver{ { 16 * 0, 16 * 1, 16, 16 }, { 16 * 1, 16 * 1, 16, 16 }, SDL_SCANCODE_RIGHT, 3 });
            m_registry.emplace<NoteReceiver>(UArrow, NoteReceiver{ { 16 * 0, 16 * 2, 16, 16 }, { 16 * 1, 16 * 2, 16, 16 }, SDL_SCANCODE_UP, 1 });
            m_registry.emplace<NoteReceiver>(DArrow, NoteReceiver{ { 16 * 0, 16 * 3, 16, 16 }, { 16 * 1, 16 * 3, 16, 16 }, SDL_SCANCODE_DOWN, 2 });



        }

        std::vector<BeatNote> beatmap;
        if (!LoadBeatmapJson("Music/mapSl.json", beatmap)) 
        {
            spdlog::error("Failed to load beatmap!");
        }

        addSystem<NoteSpawner>(m_game->music, t_Arrows, beatmap);
        //addSystem<NoteRecorder>(m_game->music); //F5
        
        
        // Char
        {
            entt::entity character = m_registry.create();
            ScreenPosition& sp = m_registry.emplace<ScreenPosition>(character, ScreenPosition{0, 720 - (120 * 2.5)});

            Sprite sChar;
            sChar.sizeX = 638; //240 * 2.5;
            sChar.sizeY = 319; //120 * 2.5;
            sChar.texture = t_char1;
            sChar.useTextureRect = true;
            sChar.layerOrder = 8;
            sChar.textureRect = {0,0,1200/5, 120};
            m_registry.emplace<Sprite>(character, sChar);

            InputTRectChange itrc;
            itrc.n = { 0,0,1200 / 5, 120 };
            itrc.u = { 240,0,1200 / 5, 120 };
            itrc.d = { 480,0,1200 / 5, 120 };
            itrc.l = { 720,0,1200 / 5, 120 };
            itrc.r = { 960,0,1200 / 5, 120 };
            itrc.dlr = { 1200,0,1200 / 5, 120 };
            m_registry.emplace<InputTRectChange>(character, itrc);
        }

        // Combo
        {
            entt::entity combo = m_registry.create();
            ScreenPosition& sp = m_registry.emplace<ScreenPosition>(combo, ScreenPosition{ 160, 30 });
            Text txt;
            txt.color = { 255,255,255,255 };
            txt.content = "0";
            txt.font = font;
            txt.fontSize = 48;
            txt.xSize = 300;
            m_registry.emplace<Text>(combo, txt);

            TextUpdateFromPointer tufp;
            tufp.pointer = &ncs->combo;
            m_registry.emplace<TextUpdateFromPointer>(combo, tufp);

            entt::entity combo1 = m_registry.create();
            ScreenPosition& sp1 = m_registry.emplace<ScreenPosition>(combo1, ScreenPosition{ 10, 30 });
            Text txt1;
            txt1.color = { 255,255,255,255 };
            txt1.content = "Combo: ";
            txt1.font = font;
            txt1.fontSize = 48;
            txt1.xSize = 300;
            m_registry.emplace<Text>(combo1, txt1);

        }
        
        // BG
        {
            entt::entity bg = m_registry.create();
            ScreenPosition& sp = m_registry.emplace<ScreenPosition>(bg, ScreenPosition{ 660 - 22, 0 });

            Sprite sprite;
            sprite.sizeX = 410;
            sprite.sizeY = 720;
            sprite.texture = t_bg;
            sprite.layerOrder = 2;
            m_registry.emplace<Sprite>(bg, sprite);
        }

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
    KitsuEngineInit(1080, 720, "OwO");

    Game* game = new Game();
    App* mainScene = new App(game);


    game->StartGame(mainScene);
    
    delete game;
    KitsuEngineClean();
    
    return 0;
}
