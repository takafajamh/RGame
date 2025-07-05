#pragma once

struct Note
{
	int column;
};

struct EndNote
{
	int column;
};

struct LongNote
{
	entt::entity startNote;
	entt::entity endNote;

	int column;
	float time;
	bool input = false;
};