#pragma once
#include <KitsuEngine/SafetyWrappers/Texture.hpp>
#include <vector>
#include <string>

struct TileInfo
{
    bool isCollider = false;
    bool isDoor = false;
    int GID = -1;
    int layer = -1;
    std::string dialogue = "";
};

struct Tilemap
{
	std::shared_ptr<Texture> texture;

    int tileWidth = 0;
    int tileHeight = 0;
    int mapWidth = 0;
    int mapHeight = 0;
    int tilesetColumns = 0;
    float scale = 2;

    std::vector<std::vector<int>> tileGIDs; // with layers accounted for 
};