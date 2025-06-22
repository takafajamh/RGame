#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>


class HeightDependendLayerSystem : public ISystem
{
public:
	void Update(entt::registry& registry) override
	{
		auto view = registry.view<HeightDependendLayer, Position, Sprite>();

		for (auto [entity, hdl, pos, sprite] : view.each())
		{
			if (hdl.playerPos == nullptr)
				continue;

			if (hdl.playerSprite == nullptr)
				continue;

			if (hdl.playerPos->y > pos.y)
			{
				sprite.layerOrder = hdl.playerSprite->layerOrder - 1;
			}
			else
			{
				sprite.layerOrder = hdl.playerSprite->layerOrder + 1;
			}
		}
	}
};