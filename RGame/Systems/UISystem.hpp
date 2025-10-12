#pragma once

#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include "../Components/ChangeSceneComponent.hpp"
#include <string>
#include "TokenSystem.hpp"

// Does not account for just position, uses Screen Position

class UISystem : public ISystem
{
private:
	Uint32 prevMouseState = 0;

	Game* m_game = nullptr;

	void checkboxUpdate(entt::registry& registry)
	{
		float mouseX, mouseY;
		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		bool justClicked = !(prevMouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) &&
			(mouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT));

		auto view = registry.view<RectangleCheckbox, RectangleShape, ScreenPosition>();

		for (auto [entity, checkbox, shape, pos] : view.each())
		{
			SDL_FRect rect = 
			{
				pos.x,
				pos.y,
				shape.width,
				shape.height
			};

			bool hovered =
				mouseX >= rect.x && mouseX <= rect.x + rect.w &&
				mouseY >= rect.y && mouseY <= rect.y + rect.h;

			// Toggle on mouse click
			if (hovered && justClicked)
			{
				checkbox.IsOn = !checkbox.IsOn;
				spdlog::info("Flipped to - {}", checkbox.IsOn);
			}

			// Set current color based on state
			if (hovered)
				shape.color = checkbox.Hover;
			else if (checkbox.IsOn)
				shape.color = checkbox.On;
			else
				shape.color = checkbox.Normal;

			prevMouseState = mouseState;
		}
	}

	void buttonUpdate(entt::registry& registry)
	{
		float mouseX, mouseY;
		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		auto view = registry.view<TextureButton, Sprite, ScreenPosition>();

		for (auto [entity, button, sprite, pos] : view.each())
		{
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

			button.isHovered = hovered;

			// Set current color based on state
			if (hovered)
			{
				sprite.textureRect = button.HoverRect;

				if (mouseState & SDL_BUTTON_LMASK)
				{
					button.isClicked = true;
					onClickEffectors(registry, entity);
				}
				else
				{
					button.isClicked = false;
					outClickEffector(registry, entity);
				}
			}
			else
			{
				sprite.textureRect = button.ClickRect;
				button.isClicked = false;
			}
				

			

		}
	}

	void onClickEffectors(entt::registry& registry, entt::entity& entity)
	{
		ChangeSceneComponent* csc = registry.try_get<ChangeSceneComponent>(entity);
		if (csc != nullptr && m_game != nullptr)
		{
			m_game->NewScene(csc->toChange);
		}

		QuitGameEffector* qge = registry.try_get<QuitGameEffector>(entity);
		if (qge != nullptr && m_game != nullptr)
		{
			m_game->StopGame();
		}

		GameVolumeEffector* gve = registry.try_get<GameVolumeEffector>(entity);
		if (gve != nullptr && m_game != nullptr && gve->unclicked)
		{
			m_game->Volume += gve->dValue;
			gve->unclicked = false;

			if (m_game->Volume > 100)
			{
				m_game->Volume -= 100;
			}

			if (m_game->Volume < 0)
			{
				m_game->Volume += 100;
			}
		}
	
		DealEffector* de = registry.try_get<DealEffector>(entity);
		if (de != nullptr)
		{
			de->clicked = true;
		}

		PassEffector* pe = registry.try_get<PassEffector>(entity);
		if (pe != nullptr)
		{
			pe->clicked = true;
		}
	}

	void outClickEffector(entt::registry& registry, entt::entity& entity)
	{
		GameVolumeEffector* ve = registry.try_get<GameVolumeEffector>(entity);
		if (ve != nullptr && m_game != nullptr)
		{
			ve->unclicked = true;
		}

		DealEffector* de = registry.try_get<DealEffector>(entity);
		if (de != nullptr && de->clicked)
		{
			de->clicked = false;
			//de->useless->call(registry);
		}

		PassEffector* pe = registry.try_get<PassEffector>(entity);
		if (pe != nullptr && pe->clicked)
		{
			pe->clicked = false;
			//pe->useless->pass(registry);
		}
	}

	void updateTexts(entt::registry& registry)
	{
		auto view = registry.view<TextUpdateFromPointer, Text>();

		for (auto [entity, tu, txt] : view.each())
		{
			txt.content = std::to_string(*tu.pointer);
		}
	}

public:
	UISystem(Game* game = nullptr)
	{
		m_game = game;
	}

	void Update(entt::registry& registry)
	{
		checkboxUpdate(registry);
		buttonUpdate(registry);
		updateTexts(registry);
	}

};