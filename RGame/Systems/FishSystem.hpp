#pragma once
#include "../Components.hpp"

class FishBrain
{
public:

	void Update(entt::registry& registry)
	{
		auto view = registry.view<Position, FishComponent>();

		for (auto[entity, position, fish] : view.each())
		{
			position.x += fish.x * dt * 200;
			position.y += fish.y * dt * 200;

			if (position.x < 290)
			{
				fish.x *= -1;
				if (auto sprite = registry.try_get<Sprite>(entity))
				{
					sprite->flipX = true;
				}
				else
				{
					spdlog::warn("An Fish does not have a Sprite!");
				}
			}
				

		}
	}
};