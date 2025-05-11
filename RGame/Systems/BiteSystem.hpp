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
public:
    std::pair<float, float> GetPosition() 
    {
        return { posx, posy };
    }

    void Update(entt::registry& registry)
    {
        const bool* keys = SDL_GetKeyboardState(nullptr);

        float dx = 0.0f, dy = 0.0f;
        if (keys[SDL_SCANCODE_LEFT])  dx -= 1.0f;
        if (keys[SDL_SCANCODE_RIGHT]) dx += 1.0f;
        if (keys[SDL_SCANCODE_UP])    dy -= 1.0f;
        if (keys[SDL_SCANCODE_DOWN])  dy += 1.0f;

        auto view = registry.view<Position, BiteComponent>();
        for (auto [entity, position, bite] : view.each())
        {
            position.x += dx * bite.speed * dt;
            position.y += dy * bite.speed * dt;

            posx = position.x;
            posy = position.y;
        }
    }


};