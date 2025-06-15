#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>

class PlayerMovementSystem : public ISystem
{
private:
	entt::entity m_player;
public:
	PlayerMovementSystem(entt::entity& Player)
	{
		m_player = Player;
	}
	void Update(entt::registry& registry) override
	{
		if (!registry.valid(m_player))
		{
			spdlog::error("Player is no longer valid, refuse to do player stuff");
			return;
		}


		constexpr float speed = 200.0f; 

		auto& pos = registry.get<Position>(m_player);

		const bool* keys = SDL_GetKeyboardState(nullptr);
		float dx = 0.0f, dy = 0.0f;
		if (keys[SDL_SCANCODE_LEFT])  dx -= 1.0f;
		if (keys[SDL_SCANCODE_RIGHT]) dx += 1.0f;
		if (keys[SDL_SCANCODE_UP])    dy -= 1.0f;
		if (keys[SDL_SCANCODE_DOWN])  dy += 1.0f;

		pos.x += dx * speed * dt;
		pos.y += dy * speed * dt;

	}
};