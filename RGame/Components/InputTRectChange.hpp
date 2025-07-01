#pragma once
#include <SDL3/SDL.h>

struct InputTRectChange
{
	SDL_FRect n;
	SDL_FRect l;
	SDL_FRect r;
	SDL_FRect u;
	SDL_FRect d;
};