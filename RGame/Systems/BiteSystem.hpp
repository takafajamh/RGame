#pragma once

#include <KitsuEngine/KitsuneEngine.hpp>
#include "../Components/Components.hpp"

class BiteSystem
{
private:
    float dx = 0.0f;
    float dy = 0.0f;

    float posx = 0;
    float posy = 0;

    entt::entity Catch = entt::null;
    float catchX = 0;
    float catchY = 0;

    std::shared_ptr<Font> font;

public:
    BiteSystem()
    {
        font = std::make_shared<Font>("Font/munro.ttf");
    }

    std::shared_ptr<Texture> t_odoru;

    std::pair<float, float> GetPosition() 
    {
        return { posx, posy };
    }

    void Update(entt::registry& registry)
    {
        if (registry.valid(Catch))
        {
            auto view = registry.view<Position, BiteComponent>();
            for (auto [entity, position, bite] : view.each())
            {
                position.x += -5 * bite.speed * dt;
                position.y += -5 * bite.speed * dt;

                if (position.y < 300)
                    position.y = 300;

                if (position.x < 300)
                    position.x = 300;


                posx = position.x;
                posy = position.y;
            
                
                if (posx == 300 && posy == 300)
                {
                    
                    auto Odoru = registry.create();

                    registry.emplace<Sprite>(Odoru,
                        Sprite{
                            (float)t_odoru->getSDL()->w / 5 * 3,
                            (float)t_odoru->getSDL()->h * 3,
                            4,
                            true,
                            {0,0,48,48}, 
                            t_odoru
                        });

                    registry.emplace<ScreenPosition>(Odoru, ScreenPosition{ 675.f, 10.f });
                    registry.emplace<OdoruTag>(Odoru, OdoruTag{ false });

                    Animation odoruAnim;
                    odoruAnim.name = "idle";

                    for (int i = 0; i < 5; ++i)
                    {
                        Frame frame;
                        frame.duration = 0.15f;
                        if (i == 2 || i == 3)
                            frame.duration = 0.2f;
                        frame.frame = SDL_FRect{ static_cast<float>(i * 48), 0.0f, 48.0f, 48.0f };
                        odoruAnim.frames.push_back(frame);
                    }

                    Animator animator;
                    animator.anims.push_back(odoruAnim);
                    animator.looping = false;
                    animator.ToPlay = "idle";
                    animator.restartOnFinish = true;

                    registry.emplace<Animator>(Odoru, animator);


                    // Points delete
                    auto Points = registry.create();

                    Text t;
                    t.color = {255,255,255,255};
                    t.content = std::to_string((int)(catchX + catchY));
                    t.xSize = t.content.length() * 24;
                    t.ySize = 80;
                    t.font = font;

                    registry.emplace<Text>(Points, t);
                    registry.emplace<ScreenPosition>(Points, ScreenPosition{350.f,20.f});
                    registry.emplace<RemoveAfterDelay>(Points, RemoveAfterDelay{1.5f, 0});


                    registry.destroy(Catch);
                    Catch = entt::null;


                }


                if (auto ppos = registry.try_get<Position>(Catch))
                {
                    ppos->x = posx + 10;
                    ppos->y = posy - 10;
                }
                else
                {
                    spdlog::warn("An Fish does not have a Position!");
                }

                if (auto sprite = registry.try_get<Sprite>(Catch))
                {
                    sprite->flipX = false;
                }
                else
                {
                    spdlog::warn("An Fish does not have a Sprite!");
                }
            
            }

            return;
        }


        const bool* keys = SDL_GetKeyboardState(nullptr);

        float dx = 0.0f, dy = 0.0f;
        if (keys[SDL_SCANCODE_LEFT])  dx -= 1.0f;
        if (keys[SDL_SCANCODE_RIGHT]) dx += 1.0f;
        if (keys[SDL_SCANCODE_UP])    dy -= 1.0f;
        if (keys[SDL_SCANCODE_DOWN])  dy += 1.0f;

        auto view = registry.view<Position, BiteComponent>();
        for (auto [entity, position, bite] : view.each())
        {
            if (position.y < 600)
                dy = 1;

            if (position.x < 320)
                dx = 1;

            position.x += dx * bite.speed * dt;
            position.y += dy * bite.speed * dt;

            posx = position.x;
            posy = position.y;


            

            auto fishView = registry.view<Position, FishComponent, Sprite>();
            for (auto [FishEntity, pos, fisher, spritee] : fishView.each())
            {
                float ddx = 0;
                if(!spritee.flipX)
                    ddx = std::abs(pos.x - position.x);
                else
                    ddx = std::abs(pos.x - position.x + (48 * 2));

                float ddy = std::abs(pos.y - position.y);

                if (ddx * ddx + ddy * ddy < 30 * 30)
                {
                    Catch = FishEntity;
                    catchX = pos.x;
                    catchY = pos.y;
                    return;
                }


            }


        }
    
    
        
    
    }


};