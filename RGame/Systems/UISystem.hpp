#pragma once

#include "../Components.hpp"

// Does not account for just position, uses Screen Position


class UISystem
{
private:
	Uint32 prevMouseState = 0;

	void CheckboxUpdate(entt::registry& registry)
	{
		float mouseX, mouseY;
		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		bool justClicked = !(prevMouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) &&
			(mouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT));

		auto view = registry.view<RectangleCheckbox, RectangleShape, ScreenPosition>();

		for (auto [entity, checkbox, shape, pos] : view.each())
		{
			SDL_FRect rect = 
			{
				pos.x,
				pos.y,
				shape.width,
				shape.height
			};

			bool hovered =
				mouseX >= rect.x && mouseX <= rect.x + rect.w &&
				mouseY >= rect.y && mouseY <= rect.y + rect.h;

			// Toggle on mouse click
			if (hovered && justClicked)
			{
				checkbox.IsOn = !checkbox.IsOn;
				spdlog::info("Flipped to - {}", checkbox.IsOn);
			}

			// Set current color based on state
			if (hovered)
				shape.color = checkbox.Hover;
			else if (checkbox.IsOn)
				shape.color = checkbox.On;
			else
				shape.color = checkbox.Normal;

			prevMouseState = mouseState;
		}
	}
public:

	void Update(entt::registry& registry)
	{
		CheckboxUpdate(registry);
	}

};