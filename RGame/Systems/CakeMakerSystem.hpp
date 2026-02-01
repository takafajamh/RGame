#pragma once

#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <string>



/*
Figure out a valid struct that would identify the cake to then do fun stuff

CakeButtonEffector
 space
 id

CakeSystem
 cakePos
 validPos
 isMoving
 vector<entt::entity> elements
 vector<rect/element> validBombs;
 vector<rect/element> validDrugs;
 ...

 ButtonClicked(button)
  if cakePos != button.space
   return with info?

  switch id
   createElement() -> calculate which, getPosition of last + some position, spawn element there with higher layer
   changeArt() -> (make hole) -> get texture rect, check if valid, change texture rect
   move() -> changePos
   loopAround() -> move, wait, move, wait, reset
   remove() -> remove all elements
   finish() -> move into first room
   changeItem() -> loop through vector, change image, change Button tip



*/

class CakeMakerSystem : public ISystem
{
private:
	int cakePos = -1;
	std::shared_ptr<Texture> t_cake;
	
	entt::entity m_cake = entt::null;
	const std::vector<Position> validPositions = {
		Position{2114,668},
		Position{2280,668},
		Position{2435,668},
		Position{2595,668},
		Position{2750,668},
		Position{2917,668},
		Position{3075,668},
		Position{3230,668},
		Position{3363,668},
		Position{3472,668},
	};

public:
	CakeMakerSystem(std::shared_ptr<Texture> texture_cake)
	{
		t_cake = texture_cake;
	}

	void Update(entt::registry& registry)
	{

	}

	void spawnCake(entt::registry& registry, int type)
	{
		spdlog::info("[CakeMakerSystem] spawned a cake");
		entt::entity cake = registry.create();
		registry.emplace<Position>(cake, Position{ 2114,668 });

		Sprite s_cake;
		s_cake.layerOrder = 3;
		s_cake.texture = t_cake;
		s_cake.sizeX = 85;
		s_cake.sizeY = 75;
		s_cake.useTextureRect = true;
		s_cake.textureRect = {(float)(type - 1) * 85, 0, 85, 75};
		registry.emplace<Sprite>(cake, s_cake);

		DebugMove dm;
		dm.key = SDL_SCANCODE_Z;
		registry.emplace<DebugMove>(cake, dm);

		m_cake = cake;
		cakePos = 1;
	}

	void moveCake(entt::registry& registry)
	{
		int pp = cakePos;
		cakePos++;

		if (pp > 0 && pp < validPositions.size())
		{
			Position& p = registry.get<Position>(m_cake);
			p.x = validPositions.at(pp).x;
			p.y = validPositions.at(pp).y;
		}
	}

	// + play sound
	void ButtonClicked(entt::registry& registry, CakeButtonEffector& cbe)
	{
		//spdlog::info("[CakeMakerSystem] {} == {}", cbe.position, cakePos);

		spdlog::info("[CakeMakerSystem] {}, {}, {}", cbe.id, cbe.position, cakePos);


		if (cbe.position != cakePos && cbe.id > 4 && cbe.id != 8) // play sound
			return;
		
		if (cbe.id <= 4 && registry.valid(m_cake))
			return;

		if (cbe.id > 4 && !registry.valid(m_cake))
			return;


		switch (cbe.id)
		{
		// Add a cake
		case 1:
		case 2:
		case 3:
		case 4:
			spawnCake(registry, cbe.id);
			break;

		// Make a hole
		case 5:
		case 6:
		case 7:
			break;

		//move
		case 8:
			moveCake(registry);
			break;

		// add stuff
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 20:
		case 23:
		case 26:
			break;


		// move R1
		case 18:
			break;

		// move L1
		case 19:
			break;

		// move R2
		case 21:
			break;

		// move L2
		case 22:
			break;

		// move R3
		case 24:
			break;

		// move L3
		case 25:
			break;

		// move up and loop
		case 27:
			break;

		// move up finish
		case 28:
			break;

		// trash
		case 29:
			break;

		default:
			kitsu_assert(false, "Wrong key ID", "Not happening, something went dramatically wrong");
			break;

		}

	}

};