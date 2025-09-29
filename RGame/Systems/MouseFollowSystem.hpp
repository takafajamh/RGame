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
			float ry = ((900 - y) * (900 - y));

			float r = std::sqrt(ry + rx);
			pos.y = 900 - r; 
			float ytan = 900 - y;
			float xtan = (x - pos.x);

			float angle = std::atan2f(xtan, ytan) * 180.0f / 3.14;
			sprite.angle = angle;
			
		}
	}
};
