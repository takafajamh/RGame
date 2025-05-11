#pragma once
#include "../Components/Components.hpp"
#include <KitsuEngine/KitsuneEngine.hpp>

class RendererSystem
{
private:
	struct Renderable
	{
		SDL_FRect dstRect;
		SDL_FRect* srcRect;
		SDL_Texture* tex;
		int layer;
	};

	std::vector<Renderable> renderQueue;

public:

	float camXPos = 0;
	float camYPos = 0;

	
	void DrawSprites(entt::registry& registry)
	{
		renderQueue.clear();
		renderQueue.reserve(registry.view<Sprite, Position>().size_hint());

		registry.view<Sprite, Position>().each([&](auto entity, Sprite& sprite, Position& position) 
			{
				SDL_FRect dstRect = { position.x - camXPos, position.y - camYPos, sprite.sizeX, sprite.sizeY };
				SDL_FRect* srcRect = nullptr;
				if (sprite.useTextureRect)
					srcRect = &sprite.textureRect;

				renderQueue.push_back({ dstRect, srcRect,sprite.texture->SDL_texture, sprite.layerOrder });
			});

		std::sort(renderQueue.begin(), renderQueue.end(), [](const auto& a, const auto& b) 
		{
			return a.layer < b.layer;
		});

		for (const auto& r : renderQueue)
		{
			bool renderState = SDL_RenderTexture(renderer, r.tex, r.srcRect, &r.dstRect);
			if (!renderState)
			{
				spdlog::critical("Failed to render texture at {}. SDL_Error: {}", static_cast<void*>(r.tex), SDL_GetError());
				assert(!renderState);
			}
		}

	}
};