#pragma once
#include <KitsuEngine/KitsuneEngine.hpp>
#include <memory>

struct Text 
{
    std::string content;
    SDL_Color color;
    std::shared_ptr<Font> font;
    float xSize;
    float ySize;
};