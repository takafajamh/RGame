#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/Globals.hpp>

class MoverSystem : public ISystem
{
public:
	void Update(entt::registry& registry) override
	{
		auto view = registry.view<Mover>();

		for (auto [entity, mv] : view.each())
		{
			Position* p = registry.try_get<Position>(entity);
			ScreenPosition* sp = registry.try_get<ScreenPosition>(entity);

			if (p != nullptr)
			{
				p->x += dt * mv.xDir * mv.Speed;
				p->y += dt * mv.yDir * mv.Speed;
			}

			if (sp != nullptr)
			{
				sp->x += dt * mv.xDir * mv.Speed;
				sp->y += dt * mv.yDir * mv.Speed;
			}

		}
	}
};