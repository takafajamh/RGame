#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "PlayerInteract.hpp"

class TimeEventManagerSystem : public ISystem
{
private:
	void MoveTime(entt::registry& registry)
	{
		time++;

		if (time >= 4)
		{
			time = 0;
			spdlog::error("BROOOO HANDLE THAT and add an pretty shit");
			return;
		}

		auto view = registry.view<NPC, Position, Sprite>();

		for (auto [entity, npc, npos, sprite] : view.each())
		{
			npc.talks = 0;
			npos.x = npc.positions.at(time).x;
			npos.y = npc.positions.at(time).y;
		}
	}

	PlayerInteractSystem* m_PI = nullptr;

	bool singleClick = false;

public:
	int time = 0;
	int dress = 0;

	TimeEventManagerSystem(PlayerInteractSystem* pi)
	{
		m_PI = pi;
	}

	void Update(entt::registry& registry)
	{
		if (m_PI == nullptr)
			return;

		m_PI->GTime = time;
		m_PI->GDress = dress;

		const bool* keys = SDL_GetKeyboardState(nullptr);
		if (keys[SDL_SCANCODE_Y] && !singleClick)
		{
			MoveTime(registry);
			singleClick = true;
			spdlog::info("Moved in time!");

		}
		if (!keys[SDL_SCANCODE_Y])
		{
			singleClick = false;
		}

	}
};