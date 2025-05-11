#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_Image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "Components/Components.hpp"
#include "Systems/RendererSystem.hpp"

#include <cassert>
#include <iostream>

class mScene : public Scene
{
public:
    RendererSystem rs;


    mScene(Game* game) : Scene(game)
    {

    }
    virtual void Init()
    {
        std::shared_ptr<Texture> t_Fumi = CreateTexture("GPX/fumi fishin.png");
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

        m_registry.emplace<Position>(Fumi, Position{ 0.f, 0.f });






        spdlog::info("Scene got init");
    }
    virtual void Update()
    {

    }
    virtual void Draw()
    {
        rs.DrawSprites(m_registry);
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
