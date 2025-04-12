#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_Image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/KitsuneEngine.hpp>

#include <cassert>


class mScene : public Scene
{
public:
    mScene(Game* game) : Scene(game)
    {

    }
    virtual void Init()
    {

    }
    virtual void Update()
    {

    }
    virtual void Draw()
    {

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
