#pragma once

#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <math.h>

class MouseFollowSystem : public ISystem
{
public:
	void Update(entt::registry& registry) override
	{
		auto view = registry.view<MouseFollowHand, ScreenPosition, Sprite>();

		for (auto [entity, mfh, pos, sprite] : view.each())
		{
			if (!mfh.follow)
				continue;

			float x, y;
			SDL_GetMouseState(&x, &y);
			

			float rx = ((x - pos.x) * (x - pos.x));
			float ry = ((1000 - y) * (1000 - y));

			float r = std::sqrt(ry + rx);
			pos.y = 1000 - r; 
			float ytan = 1000 - y;
			float xtan = (x - pos.x);

			sprite.center.y = 1000 - pos.y;

			float angle = std::atan2f(xtan, ytan) * 180.0f / 3.14;
			//mfh.angle = angle = mfh.angle + (mfh.da * dt);
			sprite.angle = angle;
			
		}
	}
};
