#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <math.h>

class NTokenSystem : public ISystem
{
public:
	bool holding = false;
	bool left = true;

	std::shared_ptr<Texture> t_Token; // required
	std::shared_ptr<Texture> t_HandToken; // required
	std::shared_ptr<Texture> t_HandNull; // required
	entt::entity hand; // required


	float distance(int x, int y, int x1, int y1)
	{
		int ax = x1 - x;
		int ay = y1 - y;

		return std::sqrt((ax * ax) + (ay * ay));
	}

	bool hovered(const int x, const int y, const ScreenPosition& pos, const Sprite& spr)
	{
		return distance(x, y, pos.x + (spr.sizeX / 2), pos.y + (spr.sizeY / 2)) < (spr.sizeY / 2);
	}

	void createToken(entt::registry& registry, ScreenPosition& pos)
	{
		entt::entity token = registry.create();
		Sprite s;
		s.sizeX = 60;
		s.sizeY = 60;
		s.texture = t_Token;
		s.layerOrder = 12;

		registry.emplace<Sprite>(token, s);
		registry.emplace<TokenComp>(token, TokenComp{});
		registry.emplace<ScreenPosition>(token, pos);
	}

	void Update(entt::registry& registry)
	{
		float x, y;
		auto mouseState = SDL_GetMouseState(&x, &y);

		auto view = registry.view<TokenComp, ScreenPosition, Sprite>();

		
		if (!holding)
		{
			if (mouseState & SDL_BUTTON_LMASK)
			{
				for (auto [entity, mc, pos, spr] : view.each())
				{
					if (hovered(x, y, pos, spr) && left)
					{
						holding = true;
						left = false;
						registry.destroy(entity);

						registry.get<Sprite>(hand).texture = t_HandToken;

						break;
					}
				}
			}
			else
			{
				left = true;
			}
		}
		else
		{
			if (mouseState & SDL_BUTTON_LMASK)
			{
				if (left)
				{
					holding = false;
					left = false;
					ScreenPosition temp;
					temp.x = x - 30;
					temp.y = y - 30;
					createToken(registry, temp);
					registry.get<Sprite>(hand).texture = t_HandNull;

				}
			}
			else
			{
				left = true;
			}
		}

			
	}

};