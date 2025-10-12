#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>

class MouseCursorSystem : public ISystem
{
public:
	void Update(entt::registry& registry)
	{
		float x, y;
		SDL_GetMouseState(&x, &y);

		auto view = registry.view<MouseCursor, ScreenPosition>();

		for (auto [entity, mc, pos] : view.each())
		{
			pos.x = x + mc.offsetX;
			pos.y = y + mc.offsetY;

			SDL_HideCursor();

		}
	}

};