#pragma once
#include <SDL3/SDL.h>

struct TimeMover
{
	SDL_FRect tRect[4];

	bool firstInteraction = true;
};