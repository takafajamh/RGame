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
			if (game != nullptr && menu != nullptr)
			{
				game->NewScene(menu);
			}
			return;
		}

		auto view = registry.view<NPC, Position, Sprite>();

		for (auto [entity, npc, npos, sprite] : view.each())
		{
			npc.talks = 0;
			npos.x = npc.positions.at(time).x;
			npos.y = npc.positions.at(time).y;
		}

		auto vieww = registry.view<TimeMover, Sprite>();

		for (auto [entity, tm, sprite] : vieww.each())
		{
			sprite.textureRect = tm.tRect[time];
		}


	}

	PlayerInteractSystem* m_PI = nullptr;

	bool singleClick = false;

public:
	int time = 0;
	int dress = 0;

	Scene* menu = nullptr;
	Game* game = nullptr;
	DialogueFlags* df = nullptr;

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

		if (m_PI->increase)
		{
			m_PI->increase = false;
			MoveTime(registry);
		}

		if (HasFlag(*df, DialogueFlag::finishFlag) && !m_PI->Interacting)
		{
			if (game != nullptr && menu != nullptr)
			{
				game->NewScene(menu);
			}
		}

#if _DEBUG
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
#endif
	}
};