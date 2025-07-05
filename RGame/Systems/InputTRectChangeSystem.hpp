#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>

class InputTRectChangeSystem : public ISystem
{
private:
	float easeOut = 0;
	const float easeTime = 0.05f;

public:

	void Update(entt::registry& registry) override
	{
		auto view = registry.view<InputTRectChange, Sprite>();

		const bool* currentKeys = SDL_GetKeyboardState(nullptr);

		for (auto [entity, itrc, spr] : view.each())
		{
			if (currentKeys[SDL_SCANCODE_LEFT] && currentKeys[SDL_SCANCODE_RIGHT])
			{
				spr.textureRect = itrc.dlr;
				easeOut = easeTime;
			}else if (currentKeys[SDL_SCANCODE_LEFT] )
			{
				spr.textureRect = itrc.l;
				easeOut = easeTime;
			}
			else if (currentKeys[SDL_SCANCODE_UP])
			{
				spr.textureRect = itrc.u;
				easeOut = easeTime;
			}
			else if (currentKeys[SDL_SCANCODE_DOWN])
			{
				spr.textureRect = itrc.d;
				easeOut = easeTime;
			}
			else if (currentKeys[SDL_SCANCODE_RIGHT])
			{
				spr.textureRect = itrc.r;
				easeOut = easeTime;
			}
			else
			{
				if (easeOut <= 0)
				{
					spr.textureRect = itrc.n;
				}
				else
				{
					easeOut -= dt;
				}
				
			}
		}
	}
};