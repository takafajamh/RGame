#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>

class InputTRectChangeSystem : public ISystem
{
public:

	void Update(entt::registry& registry) override
	{
		auto view = registry.view<InputTRectChange, Sprite>();

		const bool* currentKeys = SDL_GetKeyboardState(nullptr);

		for (auto [entity, itrc, spr] : view.each())
		{
			if (currentKeys[SDL_SCANCODE_LEFT])
			{
				spr.textureRect = itrc.l;
			}
			else if (currentKeys[SDL_SCANCODE_UP])
			{
				spr.textureRect = itrc.u;
			}
			else if (currentKeys[SDL_SCANCODE_DOWN])
			{
				spr.textureRect = itrc.d;
			}
			else if (currentKeys[SDL_SCANCODE_RIGHT])
			{
				spr.textureRect = itrc.r;
			}
			else
			{
				spr.textureRect = itrc.n;
			}
		}
	}
};