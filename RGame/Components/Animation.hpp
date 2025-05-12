#pragma once
#include <KitsuEngine/KitsuneEngine.hpp>

struct Frame
{
	SDL_FRect frame;
	float duration;
};

struct Animation
{
	std::string name;
	std::vector<Frame> frames;
};

struct Animator
{
	std::vector<Animation> anims;
	bool looping = false;
	bool restartOnFinish = false;

	std::string currentAnimationName = "";
	int currentAnimationId = -1;
	float currentTime = 0;
	int currentFrame = 0;

	std::string ToPlay = "";


};

