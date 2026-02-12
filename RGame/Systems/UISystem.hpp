#pragma once

#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include "../Components/ChangeSceneComponent.hpp"
#include "../Systems/CakeMakerSystem.hpp"
#include <string>


class UISystem : public ISystem
{
private:
	Uint32 prevMouseState = 0;
	CakeMakerSystem* cms = nullptr;
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

		auto view = registry.view<TextureButton, Sprite>();
		int width, height;
		SDL_GetWindowSize(window, &width, &height);


		for (auto [entity, button, sprite] : view.each())
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

			button.isHovered = hovered;

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

		MoveCameraEffector* mce = registry.try_get<MoveCameraEffector>(entity);
		if (mce != nullptr && mce->unclicked)
		{
			mce->unclicked = false;
		}

		if (cms != nullptr)
		{
			CakeButtonEffector* cbe = registry.try_get<CakeButtonEffector>(entity);
			if (cbe != nullptr && !cbe->clicked)
			{
				cbe->clicked = true;
				cms->ButtonClicked(registry, *cbe);
			}
		}

		OptionEffector* oe = registry.try_get<OptionEffector>(entity);
		if (oe != nullptr && !oe->clicked)
		{
			oe->clicked = true;
			oe->cur->chosenOption = oe->cur->options.at(oe->optionId);
			*oe->flagChanged = true;
		}

	}

	void outClickEffector(entt::registry& registry, entt::entity& entity)
	{
		GameVolumeEffector* ve = registry.try_get<GameVolumeEffector>(entity);
		if (ve != nullptr && m_game != nullptr)
		{
			ve->unclicked = true;
		}

		MoveCameraEffector* mce = registry.try_get<MoveCameraEffector>(entity);
		if (mce != nullptr)
		{
			if (mce->unclicked == false)
			{
				camXPos += mce->dx;
				camYPos += mce->dy;
			}
			mce->unclicked = true;
		}

		if (cms != nullptr)
		{
			CakeButtonEffector* cbe = registry.try_get<CakeButtonEffector>(entity);
			if (cbe != nullptr)
			{
				cbe->clicked = false;
			}
		}

		OptionEffector* oe = registry.try_get<OptionEffector>(entity);
		if (oe != nullptr)
		{
			oe->clicked = false;
		}
	}


public:
	UISystem(Game* game = nullptr, CakeMakerSystem* cakeSystem = nullptr)
	{
		m_game = game;
		cms = cakeSystem;
	}

	void SetCakeMakerSystem(CakeMakerSystem* system)
	{
		cms = system;
	}

	void Update(entt::registry& registry)
	{
		checkboxUpdate(registry);
		buttonUpdate(registry);
	}

};