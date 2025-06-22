#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "TilemapSystem.hpp"
#include "PlayerInteract.hpp"

class PlayerMovementSystem : public ISystem
{
private:
	entt::entity m_player = entt::null;
	TilemapSystem* m_tilemap = nullptr;
	PlayerInteractSystem* m_playerInteract = nullptr;

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
		SDL_FRect playerRect = {UL.x, UL.y, (UR.x - UL.x), (DL.y - UL.y)};

		auto view = registry.view<NPC, Position, Sprite>();

		for (auto [entity, npc, npos, sprite] : view.each())
		{
			const float x = npos.x + (sprite.sizeX / 4);
			const float y = npos.y + (sprite.sizeY / 3) * 2;
			const float w = sprite.sizeX - (sprite.sizeX / 4);
			const float h = sprite.sizeY - (sprite.sizeY / 3) * 2;
			SDL_FRect npcRect = {x, y, w, h};

			if (SDL_FRectIntersects(npcRect, playerRect))
			{
				return true;
			}
		}

		auto view2 = registry.view<TimeMover, Position, Sprite>();
		for (auto [entity, tm, npos, sprite] : view2.each())
		{
			const float x = npos.x;
			const float y = npos.y;
			const float w = sprite.sizeX;
			const float h = sprite.sizeY;
			SDL_FRect npcRect = { x, y, w, h };

			if (SDL_FRectIntersects(npcRect, playerRect))
			{
				return true;
			}
		}

		if (m_tilemap != nullptr)
		{
			std::vector<TileInfo> tiUL = m_tilemap->GetTileInfo(registry, UL.x, UL.y);
			std::vector<TileInfo> tiUR = m_tilemap->GetTileInfo(registry, UR.x, UR.y);
			std::vector<TileInfo> tiDL = m_tilemap->GetTileInfo(registry, DL.x, DL.y);
			std::vector<TileInfo> tiDR = m_tilemap->GetTileInfo(registry, DR.x, DR.y);

			return isCollider(tiDL) || isCollider(tiDR) || isCollider(tiUL) || isCollider(tiUR);
		}
		return false;
	}

	void animate(entt::registry& registry, const float& dx, const float& dy)
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


	int m_dx = 0;
	int m_dy = 0;

public:
	PlayerMovementSystem(entt::entity& Player, TilemapSystem* tilemap = nullptr)
	{
		m_player = Player;
		m_tilemap = tilemap;
	}
	
	void setInteract(PlayerInteractSystem* playerInteractSys)
	{
		m_playerInteract = playerInteractSys;
	}

	void Update(entt::registry& registry) override
	{
		if (!registry.valid(m_player))
		{
			spdlog::error("Player is no longer valid, refuse to do player stuff");
			return;
		}

		if (m_playerInteract->Interacting)
		{
			animate(registry, 0, 0);
			// Check for finish?
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
		m_playerInteract->interact(registry, dx, dy, pos);
		m_dx = dx;
		m_dy = dy;

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

	void UIRender(entt::registry& registry) override
	{
#if _DEBUG 
		Sprite& s = registry.get<Sprite>(m_player);

		Position interaction = registry.get<Position>(m_player);
		interaction.x += s.sizeX / 2;
		interaction.y += s.sizeY / 2;

		float xSpan = m_dx * (s.sizeX * 0.6f);
		float ySpan = m_dy * (s.sizeY * 0.7f);

		interaction.x += xSpan;
		interaction.y += ySpan;

		SDL_SetRenderDrawColor(renderer, 20,20,40,255);
		
		int width, height;
		SDL_GetWindowSize(window, &width, &height);

		float dx = camXPos - width / 2;
		float dy = camYPos - height / 2;
		// SDL_FRect dstRect = { position.x - dx, position.y - dy, rshape.width, rshape.height };
		
		SDL_FRect fr{ interaction.x - dx, interaction.y - dy, 6,6 };
		bool renderState = SDL_RenderFillRect(renderer, &fr);
#endif

	}
};