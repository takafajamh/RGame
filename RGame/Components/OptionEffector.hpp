#pragma once
#include "DialogueLine.hpp"

struct OptionEffector
{
	DialogueLine* cur = nullptr;
	int optionId = 0;
	bool clicked = false;
	bool* flagChanged = nullptr;
};