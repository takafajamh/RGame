#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/Globals.hpp>

class DebugSystem : public ISystem
{
public:
	void Update(entt::registry& registry) override
	{
		auto view = registry.view<Sprite, DebugPositionMove>();
		float speed = 100;

		for (auto [entity, spr, dpm] : view.each())
		{
			if (!dpm.move)
				continue;

			Position* p = registry.try_get<Position>(entity);
			ScreenPosition* sp = registry.try_get<ScreenPosition>(entity);

			const bool* keys = SDL_GetKeyboardState(nullptr);
			float dx = 0.0f, dy = 0.0f;
			if (keys[SDL_SCANCODE_LEFT])  dx -= 1.0f;
			if (keys[SDL_SCANCODE_RIGHT]) dx += 1.0f;
			if (keys[SDL_SCANCODE_UP])    dy -= 1.0f;
			if (keys[SDL_SCANCODE_DOWN])  dy += 1.0f;
			if (keys[SDL_SCANCODE_Q])  spr.angle-= dt * 10;
			if (keys[SDL_SCANCODE_R])  spr.angle += dt * 10;

			if (spr.angle > 360)
				spr.angle -= 360;

			if (spr.angle < -360)
				spr.angle += 360;

			if (p != nullptr)
			{
				p->x += dt * dx * speed;
				p->y += dt * dy * speed;

				if (dx != 0 || dy != 0)
				{
					spdlog::info("x: {}, y: {}, ang = {}", p->x, p->y, spr.angle);
				}
			}

			if (sp != nullptr)
			{
				sp->x += dt * dx * speed;
				sp->y += dt * dy * speed;

				if (dx != 0 || dy != 0)
				{
					spdlog::info("x: {}, y: {}, ang = {}", sp->x, sp->y, spr.angle);
				}
			}


			

		}
	}
};