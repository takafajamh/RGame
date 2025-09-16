#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <SDL3/SDL.h>
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
#include "Systems/TokenSystem.hpp"
#include "Systems/TrashTalkerSystem.hpp"

#include <cassert>
#include <iostream>
#include <regex>
#include <algorithm>
#include <random>


class App : public Scene
{
private:
    
    std::shared_ptr<Font> font = std::make_shared<Font>("assets/Font/munro.ttf");
    

    std::string replaceCharacters(const std::string& text, const std::vector<std::string>& names) 
    {
        std::string result = text;
        std::regex pattern(R"(\[chr(\d+)\])"); // matches [chr1], [chr2], etc.

        std::smatch match;
        auto it = result.cbegin();
        while (std::regex_search(it, result.cend(), match, pattern)) {
            int index = std::stoi(match[1]) - 1; // chr1 -> index 0
            if (index >= 0 && index < (int)names.size())
            {
                result.replace(match.position(0) + (it - result.cbegin()), match.length(0), names[index]);
                it = result.cbegin() + match.position(0) + names[index].size() + (it - result.cbegin());
            }
            else 
            {
                it = match.suffix().first; // skip if out of range
            }
        }
        return result;
    }

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
        TokenSystem* ts = addSystem<TokenSystem>();
        addSystem<UISystem>();
        addSystem<TrashTalkerSystem>();

        std::shared_ptr<Texture> t_Quest = CreateTexture("assets/GPX/Quest.png");
        std::shared_ptr<Texture> t_Party = CreateTexture("assets/GPX/Party.png");
        std::shared_ptr<Texture> t_Table = CreateTexture("assets/GPX/table.png");
        std::shared_ptr<Texture> t_Buttons = CreateTexture("assets/GPX/buttons.png");
        std::shared_ptr<Texture> t_Buttons1 = CreateTexture("assets/GPX/good.png");
        std::shared_ptr<Texture> t_Token = CreateTexture("assets/GPX/token.png");
        std::shared_ptr<Texture> t_EnemyToken1 = CreateTexture("assets/GPX/tokenE1.png");
        std::shared_ptr<Texture> t_EnemyToken2 = CreateTexture("assets/GPX/tokenE2.png");
        std::shared_ptr<Texture> t_Icons = CreateTexture("assets/GPX/icons.png");
        std::shared_ptr<Texture> t_Button = CreateTexture("assets/GPX/button.png");

        ts->t_g = t_Buttons1;
        ts->t_n = t_Buttons;

        {
            entt::entity quest = m_registry.create();
            Sprite s;
            s.sizeX = 600 / 1.5;
            s.sizeY = 800 / 1.5;
            s.texture = t_Quest;

            m_registry.emplace<Sprite>(quest, s);
            m_registry.emplace<ScreenPosition>(quest, ScreenPosition{ 100,400 });

            entt::entity text = m_registry.create();
            ts->questText = text;
            Text t;
            int r = std::rand() % ts->Quests.size();
            ts->dif = ts->Quests.at(r).dif;
            t.content = ts->Quests.at(r).s;
            t.font = font;
            t.fontSize = 24;
            t.xSize = 380;
            t.color = { 0,0,0,255 };

            m_registry.emplace<Text>(text, t);
            m_registry.emplace<ScreenPosition>(text, ScreenPosition{110, 520});

        }
        

        {
            entt::entity party = m_registry.create();
            Sprite s;
            s.sizeX = 600 / 1.5;
            s.sizeY = 800 / 1.5;
            s.texture = t_Party;

            m_registry.emplace<Sprite>(party, s);
            m_registry.emplace<ScreenPosition>(party, ScreenPosition{ 1100,400 });


            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(ts->Characters.begin(), ts->Characters.end(), g);

            entt::entity text = m_registry.create();
            ts->partyText = text;
            Text t;
            int d = std::rand() % ts->PartyMembers.size();
            ts->CDif = ts->PartyMembers.at(d).dif;
            t.content = ts->PartyMembers.at(d).t + "\n\n" + ts->replaceCharacters(ts->PartyMembers.at(d).s, ts->Characters);
            t.font = font;
            t.fontSize = 24;
            t.xSize = 420;
            t.color = { 0,0,0,255 };

            m_registry.emplace<Text>(text, t);
            m_registry.emplace<ScreenPosition>(text, ScreenPosition{ 1110, 520 });

        }

        // table
        {
            entt::entity table = m_registry.create();
            Sprite s;
            s.sizeX = 600;
            s.sizeY = 800;
            s.texture = t_Table;

            m_registry.emplace<Sprite>(table, s);
            m_registry.emplace<ScreenPosition>(table, ScreenPosition{ 510,80 });

        }

        // buttons
        {
            for (size_t i = 0; i < 8; i++)
            {
                entt::entity buttons = m_registry.create();
                Sprite s;
                s.texture = t_Buttons;
                s.layerOrder = 10;
                s.useTextureRect = true;

                switch (i)
                {
                case 0:
                    s.textureRect = {96, 96, 144, 192};
                    break;
                case 1:
                    s.textureRect = { 288, 80, 160, 192 };
                    break;
                case 2:
                    s.textureRect = { 64, 352, 128, 192 };
                    break;
                case 3:
                    s.textureRect = { 223, 320, 112, 160 };
                    break;
                case 4:
                    s.textureRect = { 352, 288, 96, 112 };
                    break;
                case 5:
                    s.textureRect = { 448, 320, 96, 192 };
                    break;
                case 6:
                    s.textureRect = { 96, 560, 128, 160 };
                    break;
                case 7:
                    s.textureRect = { 304, 512, 160, 160 };
                    break;
                }
                s.sizeX = s.textureRect.w;
                s.sizeY = s.textureRect.h;
                m_registry.emplace<ScreenPosition>(buttons, ScreenPosition{ 510 + s.textureRect.x,80 + s.textureRect.y });

                m_registry.emplace<Sprite>(buttons, s);

                ts->buttons.push_back(buttons);
                
            }
        }

        // token
        {
            for (size_t i = 0; i < 10; i++)
            {
                entt::entity token = m_registry.create();
                Sprite s;
                s.sizeX = 60;
                s.sizeY = 60;
                s.texture = t_Token;
                s.layerOrder = 13;

                m_registry.emplace<Sprite>(token, s);
                m_registry.emplace<TokenComp>(token, TokenComp{});
                m_registry.emplace<ScreenPosition>(token, ScreenPosition{ 480 + (float)(i*65),40});
            }

            entt::entity text = m_registry.create();
            Text t;
            t.content = "100";
            t.font = font;
            t.fontSize = 36;
            t.xSize = 100;
            t.color = { 0,0,0,255 };

            ts->moneyText = text;

            m_registry.emplace<Text>(text, t);
            m_registry.emplace<ScreenPosition>(text, ScreenPosition{ 1480, 10 });

        }


        // Enemy token
        {
            for (size_t i = 0; i < 6; i++)
            {
                entt::entity token = m_registry.create();
                Sprite s;
                s.sizeX = 60;
                s.sizeY = 60;
                if(i<3)
                    s.texture = t_EnemyToken1;
                else
                    s.texture = t_EnemyToken2;
                s.layerOrder = 12;

                m_registry.emplace<Sprite>(token, s);
                m_registry.emplace<EnemyTokenComp>(token, EnemyTokenComp{});
                m_registry.emplace<ScreenPosition>(token, ScreenPosition{ 1900 + (float)(i * 65),40 });
            }

        }

        constexpr int E_Y = 210;

        // add trash talker
        // Enemy icon1
        {
            entt::entity token = m_registry.create();
            Sprite s;
            s.sizeX = 60;
            s.sizeY = 60;
            s.texture = t_Icons;
            s.useTextureRect = true;
            s.textureRect = {60 * (float)(std::rand() % 6), 0, 60, 60 };
            s.layerOrder = 17;

            m_registry.emplace<Sprite>(token, s);
            m_registry.emplace<ScreenPosition>(token, ScreenPosition{1520, E_Y });


            entt::entity text = m_registry.create();
            Text t;
            t.content = "LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO";
            t.font = font;
            t.fontSize = 28;
            t.xSize = 300;
            t.color = { 0,0,0,255 };

            m_registry.emplace<TrashTalker>(text, TrashTalker{});
            m_registry.emplace<Text>(text, t);
            m_registry.emplace<ScreenPosition>(text, ScreenPosition{ 1520 - 360, E_Y + 5 });


        }

        // Enemy icon2
        {
            entt::entity token = m_registry.create();
            Sprite s;
            s.sizeX = 60;
            s.sizeY = 60;
            s.texture = t_Icons;
            s.useTextureRect = true;
            s.textureRect = { 60 * (float)(std::rand() % 6), 0, 60, 60 };
            s.layerOrder = 17;

            m_registry.emplace<Sprite>(token, s);
            m_registry.emplace<ScreenPosition>(token, ScreenPosition{ 20, E_Y });

            entt::entity text = m_registry.create();
            Text t;
            t.content = "LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO";
            t.font = font;
            t.fontSize = 28;
            t.xSize = 300;
            t.color = { 0,0,0,255 };

            m_registry.emplace<TrashTalker>(text, TrashTalker{});
            m_registry.emplace<Text>(text, t);
            m_registry.emplace<ScreenPosition>(text, ScreenPosition{ 20 + 60 + 20, E_Y + 5 });

        }

        // Deal
        {
            entt::entity button = m_registry.create();
            
            Sprite s;
            s.sizeX = 416 / 4;
            s.sizeY = 224 / 2;
            s.texture = t_Button;
            s.useTextureRect = true;
            s.layerOrder = 17;

            m_registry.emplace<Sprite>(button, s);
            m_registry.emplace<DealEffector>(button, DealEffector{ts});

            m_registry.emplace<ScreenPosition>(button, ScreenPosition{ 10, 10 });


            TextureButton tb;
            tb.HoverRect = {416/2,0,416/2, 224};
            tb.ClickRect = {0,0,416 / 2, 224 };

            m_registry.emplace<TextureButton>(button, tb);

            entt::entity text = m_registry.create();
            Text t;
            t.content = "Deal";
            t.font = font;
            t.fontSize = 28;
            t.xSize = 300;
            t.color = { 0,0,0,255 };

            
            m_registry.emplace<Text>(text, t);
            m_registry.emplace<ScreenPosition>(text, ScreenPosition{ 38, 51 });


        }

        // Pass
        {
            entt::entity button = m_registry.create();

            Sprite s;
            s.sizeX = 416 / 4;
            s.sizeY = 224 / 2;
            s.texture = t_Button;
            s.useTextureRect = true;
            s.layerOrder = 17;

            m_registry.emplace<Sprite>(button, s);
            m_registry.emplace<PassEffector>(button, PassEffector{ ts });

            m_registry.emplace<ScreenPosition>(button, ScreenPosition{ 110, 10 });


            TextureButton tb;
            tb.HoverRect = { 416 / 2,0,416 / 2, 224 };
            tb.ClickRect = { 0,0,416 / 2, 224 };

            m_registry.emplace<TextureButton>(button, tb);

            entt::entity text = m_registry.create();
            Text t;
            t.content = "Pass";
            t.font = font;
            t.fontSize = 28;
            t.xSize = 300;
            t.color = { 0,0,0,255 };


            m_registry.emplace<Text>(text, t);
            m_registry.emplace<ScreenPosition>(text, ScreenPosition{ 138, 51 });


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

Game* game;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
    srand(time(NULL));
    int res = KitsuEngineInit(1600, 900, "OwO");
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
