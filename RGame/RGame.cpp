#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_Image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "Components/Components.hpp"
#include "Systems/RendererSystem.hpp"
#include "Systems/BiteSystem.hpp"
#include "Systems/AnimatorSystem.hpp"
#include "Systems/FishSpawnerSystem.hpp"
#include "Systems/FishSystem.hpp"
#include "Systems/OdoruSystem.hpp"

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


    mScene(Game* game) : Scene(game)
    {
        bs.t_odoru = t_Odoru;
    }

    virtual void Init()
    {
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



int main(int argc, char** argv)
{
    assert(KitsuEngineInit(800,600) == 0);

    Game* game = new Game();
    mScene* mainScene = new mScene(game);


    game->StartGame(mainScene);
    

    delete game;
    KitsuEngineClean();
    
    return 0;
}
