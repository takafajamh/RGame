#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "TilemapSystem.hpp"

class PlayerMovementSystem : public ISystem
{
private:
	entt::entity m_player;
	TilemapSystem* m_tilemap;

	bool isCollider(std::vector<TileInfo> ti)
	{
		for (auto& t : ti)
		{
			if (t.isCollider)
				return true;
		}
		return ti.size() == 0;
	}

	bool shouldReturn(Position pos, entt::registry& registry)
	{
		if (m_tilemap != nullptr)
		{
			Position UL = pos;
			Position UR = pos;
			Position DL = pos;
			Position DR = pos;

			Sprite& s = registry.get<Sprite>(m_player);
			UL.y += (s.sizeY / 3) * 2;
			UR.y += (s.sizeY / 3) * 2;
			DL.y += s.sizeY;
			DR.y += s.sizeY;

			UL.x = DL.x = pos.x + (s.sizeX / 4);
			UR.x = DR.x = pos.x + s.sizeX - (s.sizeX / 4);

			std::vector<TileInfo> tiUL = m_tilemap->GetTileInfo(registry, UL.x, UL.y);
			std::vector<TileInfo> tiUR = m_tilemap->GetTileInfo(registry, UR.x, UR.y);
			std::vector<TileInfo> tiDL = m_tilemap->GetTileInfo(registry, DL.x, DL.y);
			std::vector<TileInfo> tiDR = m_tilemap->GetTileInfo(registry, DR.x, DR.y);

			return isCollider(tiDL) || isCollider(tiDR) || isCollider(tiUL) || isCollider(tiUR);
		}
		return false;
	}

	void animate(entt::registry& registry, float dx, float dy)
	{
		Animator& anim = registry.get<Animator>(m_player);

		if (dx == 0 && dy == 0)
		{
			anim.ToPlay = "Idle";
			return;
		}

		if (dy < 0)
		{
			anim.ToPlay = "Forward";
			return;
		}

		anim.ToPlay = "Backward";
	}

public:
	PlayerMovementSystem(entt::entity& Player, TilemapSystem* tilemap = nullptr)
	{
		m_player = Player;
		m_tilemap = tilemap;
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

		animate(registry, dx, dy);

		pos.x += dx * speed * dt;
		if (shouldReturn(pos, registry))
		{
			pos.x -= dx * speed * dt;
		}

		pos.y += dy * speed * dt;
		if (shouldReturn(pos, registry))
		{
			pos.y -= dy * speed * dt;
		}

	}
};