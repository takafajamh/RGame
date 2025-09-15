#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>
#include <sstream>
#include <string>
#include <iostream>

class RendererSystem : public ISystem
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


	std::vector<std::string> split(const Text& text)
	{
		std::vector<std::string> initialLines;
		std::vector<std::string> result;
		std::stringstream ss(text.content);
		std::string token;

		// Split by newlines first
		while (std::getline(ss, token, '\n'))
		{
			initialLines.push_back(token);
		}

		for (const std::string& originalLine : initialLines)
		{
			std::string current = originalLine;

			while (!current.empty())
			{
				int measuredWidth;
				size_t measuredLength;

				// Use SDL3's TTF_MeasureString to find how much text fits in the available width
				if (TTF_MeasureString(text.font->SDL_Font, current.c_str(), current.size(),
					static_cast<int>(text.xSize), &measuredWidth, &measuredLength))
				{
					// If the entire remaining text fits
					if (measuredLength >= current.size())
					{
						result.push_back(current);
						break;
					}

					// Find the best break point (prefer spaces)
					size_t breakPoint = measuredLength;

					// Look backwards from measuredLength to find a space
					size_t spacePos = current.rfind(' ', breakPoint);

					if (spacePos != std::string::npos && spacePos > 0)
					{
						breakPoint = spacePos;
					}

					std::string linePart = current.substr(0, breakPoint);
					result.push_back(linePart);

					// Skip the space if we broke at one
					if (breakPoint < current.size() && current[breakPoint] == ' ')
						breakPoint++;

					current = current.substr(breakPoint);
				}
				else
				{
					// Fallback if TTF_MeasureString fails
					if (current.size() <= 1)
					{
						result.push_back(current);
						break;
					}

					// Try with half the remaining text
					size_t fallbackLength = current.size() / 2;
					size_t spacePos = current.rfind(' ', fallbackLength);

					if (spacePos != std::string::npos && spacePos > 0)
					{
						fallbackLength = spacePos;
					}

					std::string linePart = current.substr(0, fallbackLength);
					result.push_back(linePart);

					if (fallbackLength < current.size() && current[fallbackLength] == ' ')
						fallbackLength++;

					current = current.substr(fallbackLength);
				}
			}
		}

		return result;
	}
public:
	
	virtual void Render(entt::registry& registry) override
	{
		DrawTilemaps(registry);
		DrawSprites(registry);
		DrawRectangles(registry);
		DrawTexts(registry);
	}

	void DrawTilemaps(entt::registry& registry)
	{
		int windowW, windowH;
		SDL_GetWindowSize(window, &windowW, &windowH);

		float dx = camXPos - windowW / 2;
		float dy = camYPos - windowH / 2;


		auto view = registry.view<Position, Tilemap>();
		for (auto [entity, pos, tilemap] : view.each())
		{
			if (!tilemap.texture)
			{
				spdlog::error("Tilemap does not contain a texture, continue");
				continue;
			}

			float texW, texH;
			SDL_GetTextureSize(tilemap.texture->SDL_texture, &texW, &texH);

			int tilesPerRow = (int)(texW) / tilemap.tileWidth;

			for (const auto& layer : tilemap.tileGIDs)
			{
				for (int y = 0; y < tilemap.mapHeight; ++y)
				{
					for (int x = 0; x < tilemap.mapWidth; ++x)
					{
						int idx = y * tilemap.mapWidth + x;
						int gid = layer[idx];
						if (gid == 0) continue;

						int srcX = (gid - 1) % tilesPerRow * tilemap.tileWidth;
						int srcY = (gid - 1) / tilesPerRow * tilemap.tileHeight;

						SDL_FRect srcRect = {
						static_cast<float>(srcX),
						static_cast<float>(srcY),
						static_cast<float>(tilemap.tileWidth),
						static_cast<float>(tilemap.tileHeight)
						};

						
						SDL_FRect dstRect = {
						pos.x + (float)(tilemap.scale * x * tilemap.tileWidth) - dx,
						pos.y + (float)(tilemap.scale * y * tilemap.tileHeight) - dy,
						static_cast<float>(tilemap.scale * tilemap.tileWidth),
						static_cast<float>(tilemap.scale * tilemap.tileHeight)
						};

						SDL_RenderTexture(renderer, tilemap.texture->SDL_texture, &srcRect, &dstRect);

					}
				}
			}
		}
	
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
			//auto& pos = view.get<Position>(entity);

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
			//auto& pos = view2.get<ScreenPosition>(entity);

			SDL_FRect origin = { pos.x, pos.y, (float)txt.fontSize * txt.content.size(), (float)txt.fontSize};

			std::vector<std::string> texs = split(txt);
			
			for (size_t i = 0; i < texs.size(); i++)
			{
				SDL_FRect dst = origin;
				dst.y += i * txt.fontSize + i * txt.padding;

				int textWidth, textHeight;
				TTF_SetFontSize(txt.font->getSDL(), txt.fontSize);
				TTF_GetStringSize(txt.font->getSDL(), texs.at(i).c_str(), texs.at(i).size(), &textWidth, &textHeight);

				dst.w = textWidth;

				SDL_Surface* surf = TTF_RenderText_Solid(txt.font->SDL_Font, texs.at(i).c_str(), texs.at(i).size(), txt.color);
				SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
				SDL_DestroySurface(surf);

				SDL_RenderTexture(renderer, tex, nullptr, &dst);
				SDL_DestroyTexture(tex);
			}
		}

	}

};