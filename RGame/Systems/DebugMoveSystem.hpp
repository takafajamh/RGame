#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/Globals.hpp>

class DebugMoveSystem : public ISystem
{
public:
	void Update(entt::registry& registry) override
	{
		auto view = registry.view<DebugMove, Position>();

		const bool* key_states = SDL_GetKeyboardState(nullptr);


		for (auto [entity, dm, pos] : view.each())
		{
			if (key_states[dm.key])
			{
				if (!dm.singlePress)
				{
					dm.singlePress = true;
					dm.clicked = !dm.clicked;
					spdlog::info("[DebugMoveSystem] New Position: {} {}", pos.x, pos.y);
				}
			}
			else
			{
				dm.singlePress = false;
			}

			if (key_states[SDL_SCANCODE_LSHIFT])
			{
				dm.shift = true;
			}
			else
			{
				dm.shift = false;
			}

			if (dm.clicked)
			{
				if (key_states[SDL_SCANCODE_RIGHT])
				{
					pos.x += ((int)dm.shift * 3) * dm.speed * dt + dm.speed * dt;
				}
				if (key_states[SDL_SCANCODE_LEFT])
				{
					pos.x -= ((int)dm.shift * 3) * dm.speed * dt + dm.speed * dt;
				}
				if (key_states[SDL_SCANCODE_UP])
				{
					pos.y -= ((int)dm.shift * 3) * dm.speed * dt + dm.speed * dt;
				}
				if (key_states[SDL_SCANCODE_DOWN])
				{
					pos.y += ((int)dm.shift * 3) * dm.speed * dt + dm.speed * dt;
				}
			}
		}
	}
};