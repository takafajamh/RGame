#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "TilemapSystem.hpp"
#include <vector>
#include <KitsuEngine/Globals.hpp>



class PlayerInteractSystem : public ISystem
{
private:	

	TilemapSystem* m_tilemap;
	entt::entity m_player;
	std::shared_ptr<Texture> t_Talk;
	std::shared_ptr<Font> m_font;


	std::vector<DialogueLine> m_currentDialogue;
	float m_timeForChar = 0.03f;

	float m_timer = 0;
	float m_textID = 0;
	float m_lineTime = -1;

	bool m_holdLock = false;
	
	entt::entity m_BG = entt::null;
	entt::entity m_txt = entt::null;
	entt::entity m_txt1 = entt::null;

	void addUI(entt::registry& registry)
	{
		entt::entity BG = registry.create();
		registry.emplace<ScreenPosition>(BG, ScreenPosition{ 250, 540 });

		Sprite BGsprite;
		{
			BGsprite.texture = t_Talk;
			BGsprite.useTextureRect = false;
			BGsprite.sizeX = 72 * 8;
			BGsprite.sizeY = 16 * 8;
			BGsprite.layerOrder = 1;
		}
		registry.emplace<Sprite>(BG, BGsprite);

		m_BG = BG;



		entt::entity txt = registry.create();
		registry.emplace<ScreenPosition>(txt, ScreenPosition{ 340, 550 });

		Text text;
		text.content = m_currentDialogue.at(m_textID).Name;
		text.color = { 255,255,255,255 };
		text.xSize = 64 * 8;
		text.fontSize = 24;
		text.font = m_font;

		registry.emplace<Text>(txt, text);

		m_txt = txt;


		entt::entity txt1 = registry.create();
		registry.emplace<ScreenPosition>(txt1, ScreenPosition{ 270, 580 });

		Text text1;
		text1.content = "Lorem Ipsum, Pan Tadeusz. To jest nowa linia, uwutki";
		text1.color = { 255,255,255,255 };
		text1.xSize = 86 * 8;
		text1.fontSize = 18;
		text1.font = m_font;

		registry.emplace<Text>(txt1, text1);

		m_txt1 = txt1;


	}
	
	void HandleDialogueInteraction(entt::registry& registry)
	{
		if (!IsEntityValid(registry, m_BG) || !IsEntityValid(registry, m_txt) || !IsEntityValid(registry, m_txt1))
		{
			CleanupDialogueUI(registry);
			addUI(registry);
		}
		if (m_lineTime < 0)
		{
			m_lineTime = static_cast<float>(m_currentDialogue.at(static_cast<size_t>(m_textID)).Text.length()) * m_timeForChar;
		}


		UpdateTypewriterEffect(registry);

		HandleInput();
	}

	void UpdateTypewriterEffect(entt::registry& registry)
	{
		m_timer += dt;
		float ratio = (m_timer / m_lineTime);
		if (m_timer > m_lineTime)
		{
			m_timer = m_lineTime + 1;
			ratio = 1;
		}

		registry.get<Text>(m_txt1).content = m_currentDialogue.at(m_textID).Text.substr(0, ratio * m_currentDialogue.at(m_textID).Text.size());
	}

	void HandleInput()
	{
		const bool* keys = SDL_GetKeyboardState(nullptr);

		if (keys[SDL_SCANCODE_X])
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
				m_holdLock = true;
				if (m_textID >= m_currentDialogue.size())
				{
					m_textID = 0;
					Interacting = false;
					m_currentDialogue.clear();
					m_holdLock = false;
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

	void CleanupDialogueUI(entt::registry& registry)
	{
		DestroyEntityIfValid(registry, m_BG);
		DestroyEntityIfValid(registry, m_txt);
		DestroyEntityIfValid(registry, m_txt1);
	}

	void DestroyEntityIfValid(entt::registry& registry, entt::entity& entity)
	{
		if (registry.valid(entity) && entity != entt::null)
		{
			registry.destroy(entity);
			entity = entt::null;
		}
	}

	bool IsEntityValid(const entt::registry& registry, const entt::entity& entity)
	{
		return registry.valid(m_BG) && m_BG != entt::null;
	}


public:
	bool Interacting = false;

	PlayerInteractSystem(entt::entity& Player, TilemapSystem* tilemap, std::shared_ptr<Font>& font)
	{
		m_player = Player;
		m_tilemap = tilemap;
		t_Talk = std::make_shared<Texture>("GPX/talk.png");
		m_font = font;
	}

	void interact(entt::registry& registry, const float& dx, const float& dy, const Position& pos)
	{
		if (m_tilemap != nullptr)
		{
			m_holdLock = true;
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

			auto view = registry.view<NPC, Position, Sprite>();

			SDL_FRect interactRect = { interaction.x, interaction.y, 3, 3 };


			for (auto [entity, npc, npos, sprite] : view.each())
			{
				const float x = npos.x;
				const float y = npos.y;
				const float w = sprite.sizeX;
				const float h = sprite.sizeY;
				SDL_FRect npcRect = { x, y, w, h };

				if (SDL_FRectIntersects(npcRect, interactRect))
				{
					Interacting = true;
					DialogueContext dc;
					dc.Dress = 0;
					dc.NumberOfConversarions = npc.talks;
					dc.Time = 0;
					m_currentDialogue = GetDialogue(npc, dc);
					npc.talks++;
					return;
				}
			}


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
			HandleDialogueInteraction(registry);
		}
		else
		{
			CleanupDialogueUI(registry);
		}
	}

};