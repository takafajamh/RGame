#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>

class NoteClickSystem : public ISystem
{
public:
	void Update(entt::registry& registry) override
	{
		const bool* keys = SDL_GetKeyboardState(nullptr);
		
		auto view = registry.view<NoteReceiver, Sprite>();
		for (auto [entity, nr, spr] : view.each())
		{
			if (keys[nr.button])
			{
				spr.textureRect = nr.hold;
			}
			else
			{
				spr.textureRect = nr.normal;
			}
		}
	}
};