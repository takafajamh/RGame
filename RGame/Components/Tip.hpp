#pragma once
#include <string>
#include <entt/entt.hpp>

struct Tip
{
	std::string tip = "";
	entt::entity m_text = entt::null;
	bool isHovered = false;
	float Xpadding = 30;
	float hoverTimeToShow = 2;
	float currentHover = 0;
};