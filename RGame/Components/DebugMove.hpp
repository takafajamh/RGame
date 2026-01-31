#pragma once
#include "SDL3/SDL.h"

struct DebugMove
{
	float speed = 10;
	SDL_Scancode key;
	bool clicked = false;
	bool singlePress = false;
	bool shift = false;
};