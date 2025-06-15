#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_Image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "Components.hpp"
#include "Systems/RendererSystem.hpp"
#include "Systems/BiteSystem.hpp"
#include "Systems/AnimatorSystem.hpp"
#include "Systems/FishSpawnerSystem.hpp"
#include "Systems/FishSystem.hpp"
#include "Systems/OdoruSystem.hpp"
#include "Systems/RemoveAfterDelaySystem.hpp"
#include "Systems/UISystem.hpp"

#include <cassert>
#include <iostream>

class mScene : public Scene
{
private:
    std::shared_ptr<Texture> t_Sum1;
    std::shared_ptr<Texture> t_Odoru = CreateTexture("GPX/Odddoru.png");

public:
    RendererSystem rs;
    BiteSystem bs;
    AnimatorSystem as;
    FishSpawnerSystem fss;
    FishBrain fb;
    OdoruSystem os;
    RemoveAfterDelaySystem rads;


    mScene(Game* game) : Scene(game)
    {
        bs.t_odoru = t_Odoru;
    }

    virtual void Init()
    {
        SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!audio_device)
        {
            spdlog::error("Could not instantiate the Audio Device ID, SDL Error: {}", SDL_GetError());
            assert(audio_device);
        }
        if (!Mix_OpenAudio(audio_device, nullptr)) 
        {
            spdlog::error("Could not open the audio, SDL Error: {}", SDL_GetError());
            assert(false);
        }
        Mix_Music* music = Mix_LoadMUS("Music/bg.wav");
        if (!music)
        {
            spdlog::error("Could not load the audio file, SDL Error: {}", SDL_GetError());
            assert(music);
        }
        if (!Mix_PlayMusic(music, -1)) 
        {
            spdlog::error("Could not play the music, SDL Error: {}", SDL_GetError());
            assert(false);
        }
        Mix_VolumeMusic(MIX_MAX_VOLUME / 8); // Set volume to 50%



        spdlog::info("Music started playing successfully");

        std::shared_ptr<Texture> t_Fumi = CreateTexture("GPX/fumi fishin.png");
        std::shared_ptr<Texture> t_Bite = CreateTexture("GPX/bite.png");
        t_Sum1 = CreateTexture("GPX/sum swim1.png");


        entt::entity Fumi = m_registry.create();
        m_registry.emplace<Sprite>(Fumi, 
            Sprite{
                (float)t_Fumi->getSDL()->w*6,
                (float)t_Fumi->getSDL()->h*6,
                5,
                false,
                {}, // optional, default anyway
                t_Fumi
            });

        m_registry.emplace<Position>(Fumi, Position{ 0.f, 300.f });


        entt::entity Bite = m_registry.create();
        m_registry.emplace<Sprite>(Bite,
            Sprite{
                (float)t_Bite->getSDL()->w * 3,
                (float)t_Bite->getSDL()->h * 3,
                5,
                false,
                {}, // optional, default anyway
                t_Bite
            });

        m_registry.emplace<Position>(Bite, Position{ 240.f, 370.f });
        m_registry.emplace<BiteComponent>(Bite, BiteComponent{400});


        entt::entity Water = m_registry.create();
        m_registry.emplace<Position>(Water, Position{ 300.f, 600.f });
        m_registry.emplace<RectangleShape>(Water, RectangleShape{ 120000,120000,5,{120,120,250,200} });

        entt::entity Ground = m_registry.create();
        m_registry.emplace<Position>(Ground, Position{ -500.f, 600.f });
        m_registry.emplace<RectangleShape>(Ground, RectangleShape{ 800,120000,5,{110,60,35,255} });

        entt::entity Grass = m_registry.create();
        m_registry.emplace<Position>(Grass, Position{ -500.f, 575.f });
        m_registry.emplace<RectangleShape>(Grass, RectangleShape{ 800,50,6,{120,220,100,255} });



        spdlog::info("Scene got init");
    }
    virtual void Update()
    {
        as.Update(m_registry);
        bs.Update(m_registry);
        fb.Update(m_registry);
        rads.Update(m_registry);

        rs.camXPos = bs.GetPosition().first;
        rs.camYPos = bs.GetPosition().second;

        fss.Update(Position{ bs.GetPosition().first ,bs.GetPosition().second },m_registry, t_Sum1);
        os.Update(m_registry);
    }
    virtual void Draw()
    {
        rs.DrawRectangles(m_registry);
        rs.DrawSprites(m_registry);

        float camXPos = bs.GetPosition().first;
        float camYPos = bs.GetPosition().second;

        int width, height;
        SDL_GetWindowSize(window, &width, &height);

        // Camera offset (as used in your system)
        float camOffsetX = camXPos - width / 2;
        float camOffsetY = camYPos - height / 2;

        // World position (e.g., the object's position)
        float worldX = 260;
        float worldY = 370;

        // Convert world to screen
        float screenX1 = worldX - camOffsetX;
        float screenY1 = worldY - camOffsetY;

        // Fixed screen-space point (e.g., 20px right of screen center)
        float screenX2 = width / 2 + 20;
        float screenY2 = height / 2;

        // Draw the line
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderLine(renderer, screenX1, screenY1, screenX2, screenY2);


        rs.DrawTexts(m_registry);
    }
    virtual void Events(const SDL_Event& event)
    {

    }
    ~mScene() 
    {

    }
};

class App : public Scene
{
private:
    std::shared_ptr<Font> font = std::make_shared<Font>("Font/munro.ttf");
    
public:
    RendererSystem rs;
    AnimatorSystem as;
    UISystem us;

    App(Game* game) : Scene(game)
    {
    }

    virtual void Init()
    {
        entt::entity CheckBox = m_registry.create();
        m_registry.emplace<ScreenPosition>(CheckBox, ScreenPosition{ 10.f, 10.f });
        m_registry.emplace<RectangleShape>(CheckBox, RectangleShape{ 50,50,6,{120,220,100,255} });
        m_registry.emplace<RectangleCheckbox>(CheckBox, RectangleCheckbox{{220,120,100,255},{180,180,160,255},{120,220,100,255} });

        entt::entity check_first = m_registry.create();

        Text t;
        t.color = { 255,255,255,255 };
        t.content = "Hewwo, this is a test";
        t.xSize = t.content.length() * 12;
        t.ySize = 24;
        t.font = font;

        m_registry.emplace<Text>(check_first, t);
        m_registry.emplace<ScreenPosition>(check_first, ScreenPosition{ 80.f,10.f });



        spdlog::info("Scene got init");
    }
    virtual void Update()
    {
        as.Update(m_registry);
        us.Update(m_registry);
    }
    virtual void Draw()
    {
        rs.DrawRectangles(m_registry);
        rs.DrawSprites(m_registry);

        rs.DrawTexts(m_registry);
    }
    virtual void Events(const SDL_Event& event)
    {

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
    WindowParams wp;
    //wp.Borderless = true;
    wp.AlwaysOnTop = true;
    wp.Transparent = true;
    wp.Resizable = true;
    
    assert(KitsuEngineInit(300,600,"Log", &wp ) == 0);

    if (!SDL_SetWindowPosition(window, GetScreenSize().first - 300, 50))
    {
        spdlog::error("Could not move the window: {}", SDL_GetError());
    }
    else
    {
        spdlog::info("Window moved into {},{}", GetScreenSize().first - 300, 50);
    }

    Game* game = new Game();
   // mScene* mainScene = new mScene(game);
    App* mainScene = new App(game);

    game->StartGame(mainScene);
    

    delete game;
    KitsuEngineClean();
    
    return 0;
}
