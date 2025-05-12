	#pragma once
	#include <KitsuEngine/Texture.hpp>
	#include <memory>

	struct Sprite
	{
		float sizeX;
		float sizeY;

		int layerOrder = 5;

		bool useTextureRect=false;
		SDL_FRect textureRect;

		std::shared_ptr<Texture> texture;

		bool flipX = false;

	};