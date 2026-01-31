#pragma once
#pragma once

#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include "../Components/ChangeSceneComponent.hpp"
#include <string>


class TipSystem : public ISystem
{
private:
	Uint32 prevMouseState = 0;
	std::shared_ptr<Font> m_font;

	void hoverCheck(entt::registry& registry)
	{
		float mouseX, mouseY;
		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		auto view = registry.view<Tip, Sprite>();

		int width, height;
		SDL_GetWindowSize(window, &width, &height);


		for (auto [entity, tip, sprite] : view.each())
		{
			ScreenPosition pos;

			ScreenPosition* pos_check = registry.try_get<ScreenPosition>(entity);
			if (pos_check == nullptr)
			{
				Position* pos2_check = registry.try_get<Position>(entity);
				if (pos2_check != nullptr)
				{
					float dx = camXPos - width / 2;
					float dy = camYPos - height / 2;

					pos.x = pos2_check->x - dx;
					pos.y = pos2_check->y - dy;
				}
				else
				{
					spdlog::warn("[UISystem] Button does not have position, nor screen position, ignoring");
					continue;
				}
			}
			else
			{
				pos = *pos_check;
			}

			SDL_FRect rect =
			{
				pos.x,
				pos.y,
				sprite.sizeX,
				sprite.sizeY
			};

			bool hovered =
				mouseX >= rect.x && mouseX <= rect.x + rect.w &&
				mouseY >= rect.y && mouseY <= rect.y + rect.h;

			tip.isHovered = hovered;

			if (hovered)
			{
				if (tip.currentHover >= tip.hoverTimeToShow)
				{
					if (!registry.valid(tip.m_text))
					{
						entt::entity text = registry.create();

						registry.emplace<ScreenPosition>(text, ScreenPosition{ mouseX + tip.Xpadding, mouseY });

						Text t;
						t.font = m_font;
						t.color = { 0, 0, 0, 255 };
						t.content = tip.tip;
						t.fontSize = 32;
						t.xSize = 400;
						t.ySize = 200;
						registry.emplace<Text>(text, t);

						tip.m_text = text;
					}
					else
					{
						ScreenPosition& sp = registry.get<ScreenPosition>(tip.m_text);
						sp.x = mouseX + tip.Xpadding;
						sp.y = mouseY;
					}
				}
				else
				{
					tip.currentHover += dt;
				}
				
			}
			else
			{
				tip.currentHover = 0;

				if (registry.valid(tip.m_text))
				{
					registry.destroy(tip.m_text);
					tip.m_text = entt::null;
				}
			}

		}
	}

public:
	TipSystem(std::shared_ptr<Font> font)
	{
		m_font = std::move(font);
	}

	void Update(entt::registry& registry)
	{
		hoverCheck(registry);
	}

};