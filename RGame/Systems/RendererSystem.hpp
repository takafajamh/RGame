#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>

class RendererSystem : ISystem
{
private:
	struct Renderable
	{
		SDL_FRect dstRect;
		SDL_FRect* srcRect;
		SDL_Texture* tex;
		int layer;
	};

	struct ShapeRenderable
	{
		SDL_FRect dstRect;
		int layer;
		SDL_Color color;
	};

	std::vector<Renderable> renderQueue;
	std::vector<ShapeRenderable> shapeQueue;

public:

	float camXPos = 0;
	float camYPos = 0;

	
	void Render(entt::registry& registry)
	{

	}

	void DrawSprites(entt::registry& registry)
	{
		renderQueue.clear();
		renderQueue.reserve(registry.view<Sprite, Position>().size_hint() + registry.view<Sprite, ScreenPosition>().size_hint());

		int width, height;
		SDL_GetWindowSize(window, &width, &height);

		float dx = camXPos - width / 2;
		float dy = camYPos - height / 2;

		registry.view<Sprite, Position>().each([&](auto entity, Sprite& sprite, Position& position) 
			{
				SDL_FRect dstRect = { position.x - dx, position.y - dy, sprite.sizeX, sprite.sizeY };
				if (sprite.flipX)
				{
					dstRect.x += dstRect.w;
					dstRect.w *= -1;
				}

				SDL_FRect* srcRect = nullptr;
				if (sprite.useTextureRect)
					srcRect = &sprite.textureRect;

				renderQueue.push_back({ dstRect, srcRect,sprite.texture->SDL_texture, sprite.layerOrder });
			});

		registry.view<Sprite, ScreenPosition>().each([&](auto entity, Sprite& sprite, ScreenPosition& sposition)
			{
				SDL_FRect dstRect = { sposition.x, sposition.y, sprite.sizeX, sprite.sizeY };
				if (sprite.flipX)
				{
					dstRect.x += dstRect.w;
					dstRect.w *= -1;
				}
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

	void DrawRectangles(entt::registry& registry)
	{
		shapeQueue.clear();
		shapeQueue.reserve(registry.view<RectangleShape, Position>().size_hint() + registry.view<RectangleShape, ScreenPosition>().size_hint());

		int width, height;
		SDL_GetWindowSize(window, &width, &height);

		float dx = camXPos - width / 2;
		float dy = camYPos - height / 2;

		registry.view<RectangleShape, Position>().each([&](auto entity, RectangleShape& rshape, Position& position)
			{
				SDL_FRect dstRect = { position.x - dx, position.y - dy, rshape.width, rshape.height };

				shapeQueue.push_back({dstRect,rshape.layer,rshape.color});
			});

		registry.view<RectangleShape, ScreenPosition>().each([&](auto entity, RectangleShape& rshape, ScreenPosition& sposition)
			{
				SDL_FRect dstRect = { sposition.x, sposition.y, rshape.width, rshape.height };

				shapeQueue.push_back({ dstRect,rshape.layer,rshape.color });
			});

		std::sort(shapeQueue.begin(), shapeQueue.end(), [](const auto& a, const auto& b)
			{
				return a.layer < b.layer;
			});

		for (const auto& r : shapeQueue)
		{
			SDL_SetRenderDrawColor(renderer, r.color.r, r.color.g, r.color.b, r.color.a);
			bool renderState = SDL_RenderFillRect(renderer, &r.dstRect);

			if (!renderState)
			{
				spdlog::critical("Failed to render Rectangle Shape. SDL_Error: {}", SDL_GetError());
				assert(!renderState);
			}
		}

	}

	void DrawTexts(entt::registry& registry)
	{
		auto view = registry.view<Text, Position>();

		int width, height;
		SDL_GetWindowSize(window, &width, &height);

		float dx = camXPos - width / 2;
		float dy = camYPos - height / 2;

		for (auto [entity, txt, pos] : view.each())
		{
			auto& text = view.get<Text>(entity);
			auto& pos = view.get<Position>(entity);

			SDL_FRect dst = { pos.x - dx, pos.y - dy, txt.xSize, txt.ySize};

			SDL_Surface* surf = TTF_RenderText_Solid(txt.font->SDL_Font, text.content.c_str(), text.content.size(), txt.color);
			SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
			SDL_DestroySurface(surf);

			SDL_RenderTexture(renderer, tex, nullptr, &dst);
			SDL_DestroyTexture(tex);
		}



		auto view2 = registry.view<Text, ScreenPosition>();

		for (auto [entity, txt, pos] : view2.each())
		{
			auto& text = view2.get<Text>(entity);
			auto& pos = view2.get<ScreenPosition>(entity);

			SDL_FRect dst = { pos.x, pos.y, txt.xSize, txt.ySize };

			SDL_Surface* surf = TTF_RenderText_Solid(txt.font->SDL_Font, text.content.c_str(), text.content.size(), txt.color);
			SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
			SDL_DestroySurface(surf);

			SDL_RenderTexture(renderer, tex, nullptr, &dst);
			SDL_DestroyTexture(tex);
		}

	}

};