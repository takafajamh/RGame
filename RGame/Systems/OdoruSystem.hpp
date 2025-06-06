#pragma once
#include <KitsuEngine/KitsuneEngine.hpp>
#include "../Components.hpp"


class OdoruSystem
{
public:

	void Update(entt::registry& registry)
	{
		auto view = registry.view<Animator, OdoruTag>();

		for (auto [entity, anim, o] : view.each())
		{
			if (anim.currentAnimationId == -1 && anim.ToPlay == "")
			{
				registry.destroy(entity);
			}
		}

	}
};