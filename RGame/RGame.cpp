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
        addSystem<NoteClickSystem>();

        std::shared_ptr<Texture> t_Arrows = CreateTexture("GPX/Arrows.png");

        // Arrows down
        {
            entt::entity LArrow = m_registry.create();
            entt::entity RArrow = m_registry.create();
            entt::entity UArrow = m_registry.create();
            entt::entity DArrow = m_registry.create();

            float m = 120;

            m_registry.emplace<ScreenPosition>(LArrow, ScreenPosition{ 260, 600 });
            m_registry.emplace<ScreenPosition>(UArrow, ScreenPosition{ 260 + m, 600 });
            m_registry.emplace<ScreenPosition>(DArrow, ScreenPosition{ 260 + 2 * m, 600 });
            m_registry.emplace<ScreenPosition>(RArrow, ScreenPosition{ 260 + 3 * m, 600 });

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


            m_registry.emplace<NoteReceiver>(LArrow, NoteReceiver{ { 16 * 0, 16 * 0, 16, 16 }, { 16 * 1, 16 * 0, 16, 16 }, SDL_SCANCODE_LEFT });
            m_registry.emplace<NoteReceiver>(RArrow, NoteReceiver{ { 16 * 0, 16 * 1, 16, 16 }, { 16 * 1, 16 * 1, 16, 16 }, SDL_SCANCODE_RIGHT });
            m_registry.emplace<NoteReceiver>(UArrow, NoteReceiver{ { 16 * 0, 16 * 2, 16, 16 }, { 16 * 1, 16 * 2, 16, 16 }, SDL_SCANCODE_UP });
            m_registry.emplace<NoteReceiver>(DArrow, NoteReceiver{ { 16 * 0, 16 * 3, 16, 16 }, { 16 * 1, 16 * 3, 16, 16 }, SDL_SCANCODE_DOWN });



        }

        std::vector<BeatNote> beatmap;
        if (!LoadBeatmapJson("Music/map.json", beatmap)) 
        {
            spdlog::error("Failed to load beatmap!");
        }

        addSystem<NoteSpawner>(m_game->music, t_Arrows, beatmap);
        //addSystem<NoteRecorder>(m_game->music);


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
