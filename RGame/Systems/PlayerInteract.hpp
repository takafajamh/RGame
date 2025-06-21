#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "TilemapSystem.hpp"
#include <vector>

struct DialogueLine
{
	std::string Name;
	std::string Text;
};


class PlayerInteractSystem : public ISystem
{
private:
	TilemapSystem* m_tilemap;
	entt::entity m_player;

	std::vector<DialogueLine> m_currentDialogue;
	float m_timeForChar = 0.05f;

	float m_timer = 0;
	float m_textID = 0;
	float m_lineTime = -1;

	bool m_holdLock = false;

public:
	bool Interacting = false;

	PlayerInteractSystem(entt::entity& Player, TilemapSystem* tilemap = nullptr)
	{
		m_player = Player;
		m_tilemap = tilemap;
	}

	void interact(entt::registry& registry, const float& dx, const float& dy, const Position& pos)
	{
		if (m_tilemap != nullptr)
		{
			const bool* keys = SDL_GetKeyboardState(nullptr);
			if (!keys[SDL_SCANCODE_Z])
			{
				return;
			}

			Sprite& s = registry.get<Sprite>(m_player);

			Position interaction = pos;
			interaction.x += s.sizeX / 2;
			interaction.y += s.sizeY / 2;

			float xSpan = dx * (s.sizeX * 0.6f);
			float ySpan = dy * (s.sizeY * 0.7f);

			interaction.x += xSpan;
			interaction.y += ySpan;

			std::vector<TileInfo> ti = m_tilemap->GetTileInfo(registry, interaction.x, interaction.y);
			for (const TileInfo& t : ti)
			{
				if (t.dialogue != "")
				{
					Interacting = true;
					m_currentDialogue.push_back(DialogueLine{"You", t.dialogue});
					return;
				}
			}
		}
	}

	void Update(entt::registry& registry) override
	{
		if (Interacting)
		{
			const bool* keys = SDL_GetKeyboardState(nullptr);


			if (m_lineTime < 0)
			{
				m_lineTime = static_cast<float>(m_currentDialogue.at(static_cast<size_t>(m_textID)).Text.length()) * m_timeForChar;
			}

			m_timer += dt;
			if (m_timer > m_lineTime)
			{
				m_timer = m_lineTime + 1;
			}
			if (keys[SDL_SCANCODE_Z] && !m_holdLock)
			{
				if (m_timer > m_lineTime)
				{
					m_timer = 0;
					m_lineTime = -1;
					m_textID++;
					if (m_textID >= m_currentDialogue.size())
					{
						m_textID = 0;
						Interacting = false;
						m_currentDialogue.clear();
					}
				}
				else
				{
					m_timer = m_lineTime + 1;
					m_holdLock = true;
				}
			}
			else if (!keys[SDL_SCANCODE_Z])
			{
				m_holdLock = false;
			}
			
		}
	}

	void UIRender(entt::registry& registry) override
	{
		//if(Interacting)
		// Add Text Entity
		// Add BG
		// else if valid entities
		// Remove them
			
	}
};