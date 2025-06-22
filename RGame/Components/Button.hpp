#pragma once
#include <KitsuEngine/KitsuneEngine.hpp>
#include <KitsuEngine/SafetyWrappers/Texture.hpp>

// Requires Sprite
struct TextureButton
{
	SDL_FRect HoverRect;
	SDL_FRect ClickRect;

	bool isHovered = false;
	bool isClicked = false;
};