#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>

#include <SDL3_Image/SDL_image.h>
#include <SDL3/SDL.h>
#include <fstream>



class TilemapSystem : public ISystem
{
private:
	std::unordered_map<int, TileInfo> tileInfoLookup;

public:
	entt::entity LoadMap(entt::registry& registry, const std::string& jsonPath, std::shared_ptr<Texture>& tilesheet, Position p)
	{
		using json = nlohmann::json;

		std::ifstream file(jsonPath);
		if (!file.is_open())
		{
			spdlog::error("File is not open, path may be incorrect, path - {}", jsonPath);
			return entt::null;
		}

		json mapData;
		file >> mapData;

		int specialCount = 0;

		if (!mapData.contains("layers") || !mapData["layers"].is_array())
		{
			spdlog::error("Invalid map file: missing or invalid 'layers' array.");
			return entt::null;
		}

		if (!mapData.contains("width") || !mapData.contains("height") || !mapData.contains("tilewidth") || !mapData.contains("tileheight")) 
		{
			spdlog::error("Invalid map file: missing basic map dimensions.");
			return entt::null;
		}

		if (!mapData.contains("tilesets") || !mapData["tilesets"].is_array())
		{
			spdlog::warn("Map does not contain any additional information regarding tilesets, or they are incorrectly formatted, is it delibarate?.");
		}
		else
		{
			int firstGid = 0;
			
			for (const auto& tileset : mapData["tilesets"])
			{
				if (!tileset.contains("firstgid") || !tileset.contains("tiles")) continue;
				firstGid = tileset["firstgid"];

				for (const auto& tile : tileset["tiles"])
				{
					int localId = tile["id"];
					int gid = firstGid + localId;

					TileInfo info{};
					info.GID = gid;

					if (tile.contains("properties") && tile["properties"].is_array())
					{
						for (const auto& prop : tile["properties"])
						{
							if (prop["name"] == "Collider" && prop["type"] == "bool")
								info.isCollider = prop["value"];

							if (prop["name"] == "Door" && prop["type"] == "bool")
								info.isDoor = prop["value"];

							if (prop["name"] == "Desc" && prop["type"] == "string")
								info.dialogue = prop["value"];
						}
					}

					if (info.isCollider || info.isDoor || info.dialogue != "")
					{
						tileInfoLookup[gid] = info;
						++specialCount;
					}
				}
			}
		}
		


		entt::entity mapEntity = registry.create();
		auto& tilemap = registry.emplace<Tilemap>(mapEntity);
		auto& position = registry.emplace<Position>(mapEntity, p);

		tilemap.mapWidth = mapData["width"];
		tilemap.mapHeight = mapData["height"];
		tilemap.tileWidth = mapData["tilewidth"];
		tilemap.tileHeight = mapData["tileheight"];
		for (const auto& layer : mapData["layers"]) 
		{
			if (layer["type"] != "tilelayer") continue;

			std::vector<int> layerVec = layer["data"].get<std::vector<int>>();

			tilemap.tileGIDs.push_back(std::move(layerVec));
		}

		tilemap.texture = tilesheet;

		// Log summary
		spdlog::info("[Tilemap Loaded] Size: {}x{}, TileSize: {}x{}, SpecialTiles: {}",
			tilemap.mapWidth, tilemap.mapHeight,
			tilemap.tileWidth, tilemap.tileHeight,
			specialCount
		);

		return mapEntity;
	}

	std::vector<TileInfo> GetTileInfo(const entt::registry& registry, const float x, const float y)
	{
		std::vector<TileInfo> tilesInfo;

		auto view = registry.view<Position, Tilemap>();

		for (auto [entity, pos, tilemap] : view.each())
		{
			// World-to-local offset
			float localX = x - pos.x;
			float localY = y - pos.y;

			// Scaled tile dimensions
			float tileW = tilemap.tileWidth * tilemap.scale;
			float tileH = tilemap.tileHeight * tilemap.scale;

			if (localX < 0 || localY < 0) continue;

			int tileX = static_cast<int>(localX / tileW);
			int tileY = static_cast<int>(localY / tileH);

			// Bounds check
			if (tileX < 0 || tileY < 0 || tileX >= tilemap.mapWidth || tileY >= tilemap.mapHeight)
				continue;

			int index = tileY * tilemap.mapWidth + tileX;

			for (size_t layerIndex = 0; layerIndex < tilemap.tileGIDs.size(); ++layerIndex)
			{
				const auto& layer = tilemap.tileGIDs[layerIndex];
				if (index >= static_cast<int>(layer.size())) continue;

				int gid = layer[index];
				if (gid == 0) continue;

				TileInfo info;
				auto found = tileInfoLookup.find(gid);
				if (found != tileInfoLookup.end())
				{
					info = found->second;
				}
				else
				{
					info = TileInfo{}; // default fallback
				}

				info.GID = gid;
				info.layer = static_cast<int>(layerIndex);

				tilesInfo.push_back(info);
			}
		}

		return tilesInfo;
	}

};