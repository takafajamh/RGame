#pragma once

#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <string>

struct CakeComponentFlag
{
	const bool isDelicious = true;
};

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
	const int cakeSize = 28;
	
	int cakePos = -1;
	int height = 0;
	int layer = 3;
	
	
	std::shared_ptr<Texture> t_cake;
	std::shared_ptr<Texture> t_icing;
	


	std::vector<entt::entity> elements;
	
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
	CakeMakerSystem(std::shared_ptr<Texture> texture_cake, std::shared_ptr<Texture> texture_icing)
	{
		t_cake = texture_cake;
		t_icing = texture_icing;
	}

	void Update(entt::registry& registry)
	{

	}

	void spawnCake(entt::registry& registry, int type)
	{
		spdlog::info("[CakeMakerSystem] spawned a cake");
		entt::entity cake = registry.create();
		registry.emplace<Position>(cake, Position{ 2114,668 - (float)(height-1) * cakeSize });

		Sprite s_cake;
		s_cake.layerOrder = layer;
		s_cake.texture = t_cake;
		s_cake.sizeX = 85;
		s_cake.sizeY = 75;
		s_cake.useTextureRect = true;
		s_cake.textureRect = {(float)(type - 1) * 85, 0, 85, 75};
		registry.emplace<Sprite>(cake, s_cake);

		registry.emplace<CakeComponentFlag>(cake, CakeComponentFlag{});

		elements.push_back(cake);
		cakePos = 1;
		height++;
		layer++;
	}

	void moveCake(entt::registry& registry)
	{
		int pp = cakePos;
		cakePos++;

		if (pp > 0 && pp < validPositions.size())
		{
			for (entt::entity& e : elements)
			{
				Position& ppp = registry.get<Position>(e);
				ppp.x = validPositions.at(pp).x;
			}
		}
	}

	// fucking literally copy for added elements, but spawn like 5 at random in a circle - https://quick-bench.com/q/4YLsNRGjt5kd-IfRtizk99Cf0u4
	void addIcing(entt::registry& registry, int type)
	{
		spdlog::info("[CakeMakerSystem] spawned icing");

		entt::entity icing = registry.create();
		registry.emplace<Position>(icing, Position{ 2435,694 - (float)(height - 1) * cakeSize });

		Sprite s_ice;
		s_ice.layerOrder = layer;
		s_ice.texture = t_icing;
		s_ice.sizeX = 85;
		s_ice.sizeY = 75;
		s_ice.useTextureRect = true;
		s_ice.textureRect = { (float)(type) * 85, 0, 85, 75 };
		registry.emplace<Sprite>(icing, s_ice);

		elements.push_back(icing);

		DebugMove dm;
		dm.key = SDL_SCANCODE_X;
		registry.emplace<DebugMove>(icing, dm);
		layer++;
	}

	void trash(entt::registry& registry)
	{
		for (entt::entity& e : elements)
		{
			registry.destroy(e);
		}
		elements.clear();
		height = 0;
		cakePos = -1;
	}

	void loop(entt::registry& registry)
	{
		cakePos = 1;
		int pp = cakePos - 1;

		if (pp >= 0 && pp < validPositions.size())
		{
			for (entt::entity& e : elements)
			{
				Position& ppp = registry.get<Position>(e);
				ppp.x = validPositions.at(pp).x;
			}
		}

	}

	// I can make hole, what about the icing? or the toping, texture mask?
	void makeAHole(entt::registry& registry, int type)
	{
		// type - 1, 2, 3

		for (int i = elements.size() - 1; i >= 0; i--)
		{
			CakeComponentFlag* ccf = registry.try_get<CakeComponentFlag>(elements.at(i));
			if (ccf != nullptr)
			{
				Sprite& s = registry.get<Sprite>(elements.at(i));
				int remap[] = { 1, 3, 2 };
				int y = remap[type - 1] * s.textureRect.h;
				s.textureRect.y = y;
				break;
			}
		}



	}

	void finish(entt::registry& registry)
	{
		int h = 0;
		for (size_t i = 0; i < elements.size(); i++)
		{
			entt::entity& e = elements.at(i);

			// move it into a chat / order manager
			registry.get<Position>(e).x = 400;
			registry.get<Position>(e).y -= 165;

			Sprite& s = registry.get<Sprite>(e);
			s.sizeX *= 2;
			s.sizeY *= 2;

			if (registry.try_get<CakeComponentFlag>(e) != nullptr && i != 0)
				h++;

			registry.get<Position>(e).y -= cakeSize * h;

			DebugMove dm;
			dm.key = SDL_SCANCODE_Y;
			registry.emplace_or_replace<DebugMove>(e, dm);
		}

		elements.clear();
		height = 0;
		cakePos = -1;
	}

	// + play sound
	void ButtonClicked(entt::registry& registry, CakeButtonEffector& cbe)
	{
		//spdlog::info("[CakeMakerSystem] {} == {}", cbe.position, cakePos);

		spdlog::info("[CakeMakerSystem] {}, {}, {}", cbe.id, cbe.position, cakePos);


		if (cbe.position != cakePos && cbe.id > 4 && cbe.id != 8) // I am not on the same tile as button and I am not 4 create buttons, nor am I move button
			return; 
		
		if (cbe.id <= 4 && elements.size() > 0 && cbe.position != cakePos) // I am one of the early buttons, I have many elements and I am not on the same position
			return;

		if (cbe.id > 4 && elements.size() == 0)
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
			makeAHole(registry, cbe.id - 4);
			break;

		//move
		case 8:
			moveCake(registry);
			break;

		// add stuff
		case 9:
		case 10:
		case 11:
			addIcing(registry, cbe.id - 9);
			break;
			
			// addons, just add graphics :v
		case 12: //
		case 13: //
		case 14: //
		case 15: ///
		case 16: ///
		case 17: ///
		case 20: ////
		case 23: /////
		case 26: //////
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
			loop(registry);
			break;

		// move up finish, remove from there and put for somebody else??
		case 28:
			finish(registry);
			break;

		// trash
		case 29:
			trash(registry);
			break;

		default:
			kitsu_assert(false, "Wrong key ID", "Not happening, something went dramatically wrong");
			break;

		}

	}

};