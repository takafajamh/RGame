#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>

class NoteClickSystem : public ISystem
{
private:
    std::vector<bool> prevKeys;
    float m = 100;
    float sPos = 660;

public:
    int combo = 0;

    NoteClickSystem()
    {
        prevKeys.resize(SDL_SCANCODE_COUNT, 0);
    }

    void Update(entt::registry& registry) override
    {
        const bool* currentKeys = SDL_GetKeyboardState(nullptr);

        const float hitY = 600.0f;
        const float hitWindow = 50.0f;
        const float missWindow = 70.0f;

        auto view = registry.view<NoteReceiver, Sprite>();

        for (auto [entity, nr, spr] : view.each())
        {
            SDL_Scancode key = nr.button;

            // Check if key was JUST pressed (down this frame, up last frame)
            if (currentKeys[key] && !prevKeys[key])
            {
                spr.textureRect = nr.hold;

                auto notes = registry.view<ScreenPosition, Mover, Note>();
                for (auto [noteEntity, sp, mv, nt] : notes.each())
                {
                    int col = (int)((sp.x - sPos) / m);
                    if (col == nr.column)
                    {
                        float dy = std::abs(sp.y - hitY);
                        if (dy <= hitWindow)
                        {
                            registry.destroy(noteEntity);
                            combo++;
                            spdlog::info("COMBO: {}", combo);
                            break; 
                        }
                        else if (dy <= missWindow)
                        {
                            registry.destroy(noteEntity);
                            combo = 0;
                            spdlog::warn("COMBO: 0 - RESET");
                            break;
                        }
                    }
                }
            }
            else if (currentKeys[key])
            {
                spr.textureRect = nr.hold;
            }
            else
            {
                spr.textureRect = nr.normal;
            }
        }

        // Update previous key state for next frame
        std::copy(currentKeys, currentKeys + SDL_SCANCODE_COUNT, prevKeys.begin());
    }
};
