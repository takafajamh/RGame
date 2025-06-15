#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>

class RemoveAfterDelaySystem : public ISystem
{
public:

	void Update(entt::registry& registry)
	{
		auto view = registry.view<RemoveAfterDelay>();

		for (auto [entity, rad] : view.each())
		{
			rad.current += dt;
			if (rad.current >= rad.delay)
			{
				registry.destroy(entity);
			}
		}
	}
};