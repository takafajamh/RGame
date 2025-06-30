#pragma once
#include <SDL3/SDL.h>

struct NoteReceiver
{
	SDL_FRect normal;
	SDL_FRect hold;
	SDL_Scancode button;
	int column;
};