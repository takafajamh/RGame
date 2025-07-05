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


    const float hitY = 600.0f;
    const float hitWindow = 50.0f;
    const float missWindow = 70.0f;
    
    void handleLongNotesStart(entt::registry& registry, const NoteReceiver& nr)
    {
        auto notes = registry.view<ScreenPosition, Mover, LongNote, RectangleShape>(); // TO SPRITE LATER
        for (auto [noteEntity, sp, mv, nt, rs] : notes.each())
        {
            int col = (int)((sp.x - sPos) / m);
            if (col == nr.column)
            {
                if (!nt.input)
                {
                    const ScreenPosition& nSp = registry.get<ScreenPosition>(nt.startNote);

                    float dy = std::abs(nSp.y - hitY);
                    if (dy <= hitWindow)
                    {
                        registry.destroy(nt.startNote);
                        combo++;
                        nt.input = true;
                        break;
                    }
                    else if (dy <= missWindow)
                    {
                        registry.destroy(nt.startNote);
                        registry.destroy(nt.endNote);
                        registry.destroy(noteEntity);
                        combo = 0;
                        break;
                    }
                }
            }
        }
    }

    void handleLongNotesFinish(entt::registry& registry, const NoteReceiver& nr)
    {
        auto notes = registry.view<ScreenPosition, Mover, LongNote, RectangleShape>(); // TO SPRITE LATER

        for (auto [noteEntity, sp, mv, nt, rs] : notes.each())
        {
            int col = (int)((sp.x - sPos) / m);
            if (col == nr.column)
            {
                if (nt.input)
                {
                    const ScreenPosition& nSp = registry.get<ScreenPosition>(nt.endNote);
                    float dy = std::abs(nSp.y - hitY);

                    if (dy <= hitWindow)
                    {
                        registry.destroy(nt.endNote);
                        registry.destroy(noteEntity);
                        combo++;
                        break;
                    }
                    else
                    {
                        registry.destroy(nt.endNote);
                        registry.destroy(noteEntity);
                        combo = 0;
                        break;
                    }
                }
            }
        }
    }

    void handleNotes(entt::registry& registry, const NoteReceiver& nr)
    {
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
                    break;
                }
                else if (dy <= missWindow)
                {
                    registry.destroy(noteEntity);
                    combo = 0;
                    break;
                }
            }
        }
    }

public:
    int combo = 0;

    NoteClickSystem()
    {
        prevKeys.resize(SDL_SCANCODE_COUNT, 0);
    }

    void Update(entt::registry& registry) override
    {
        const bool* currentKeys = SDL_GetKeyboardState(nullptr);

        auto view = registry.view<NoteReceiver, Sprite>();

        for (auto [entity, nr, spr] : view.each())
        {
            SDL_Scancode key = nr.button;
            
            if (currentKeys[key])
            {
                spr.textureRect = nr.hold;
            }
            else
            {
                spr.textureRect = nr.normal;
            }

            // Check if key was JUST pressed (down this frame, up last frame)
            if (currentKeys[key] && !prevKeys[key])
            {
                handleLongNotesStart(registry, nr);
                handleNotes(registry, nr);
            }

            // Check if key was JUST released
            if (!currentKeys[key] && prevKeys[key])
            {
                handleLongNotesFinish(registry, nr);
            }
        }

        // Update previous key state for next frame
        std::copy(currentKeys, currentKeys + SDL_SCANCODE_COUNT, prevKeys.begin());
    }
};
