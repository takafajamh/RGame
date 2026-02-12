#pragma once
#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/Globals.hpp>
#include "DialogueSystem.hpp"
#include "CakeMakerSystem.hpp"
#include <queue>



struct Client
{
	int item = -1;
	int type = 0;
	int special = 0;
	DialogueLine* start;
	DialogueLine* endLoop;
	DialogueLine* correct;
	DialogueLine* incorrect;
	Sprite spr;
	Position pos;

	entt::entity character;
};

class GameManager : public ISystem
{
private:


	std::vector<entt::entity> cake;
	Client* cur = nullptr;
	DialogueSystem* ds;
	CakeMakerSystem* cms;



	void destroyCake(entt::registry& registry)
	{
		for (entt::entity& e : cake)
		{
			registry.destroy(e);
		}
		cake.clear();
	}

	void destroyCharacter(entt::registry& registry)
	{
		if (!cur)
			return;


		if (registry.valid(cur->character))
		{
			registry.destroy(cur->character);
			spdlog::info("[GameManager] removed character");
			delete cur;
			cur = nullptr;
		}
		else
		{
			//kitsu_assert(true, "WTF happened here, the character is removed again", "Impossible");
			// Actually it can happen, the first -. naah
		}
	}

	bool checkValid(entt::registry& registry) // TODO
	{
		return true;
	}

	void spawnNewCharacter(entt::registry& registry)
	{
		entt::entity ch = registry.create();
		registry.emplace<Sprite>(ch, cur->spr);
		registry.emplace<Position>(ch, cur->pos);

		cur->character = ch;
	}


public:
	std::queue<Client*> clients;
	

	GameManager(DialogueSystem* _ds, CakeMakerSystem* _cms)
	{
		ds = _ds;
		cms = _cms;
	}

	

	void Update(entt::registry& registry) override
	{

		if (cms->StealElements.size() != 0)
		{
			spdlog::info("[GameManager] Cake has been deposited");

			destroyCake(registry);
			cake = cms->StealElements;
			cms->StealElements.clear();

			if (cur && cur->correct)
			{
				if (checkValid(registry))
				{
					cur->endLoop->next = cur->correct;

				}
				else
				{
					cur->endLoop->next = cur->incorrect;
				}
			}
		}

		if (!ds->running) // it ended dialogue, because I loop it, the cake would already exist
		{
			if (clients.size() == 0)
			{
				//end;
				spdlog::critical("Throw unimplemented baka");
				return;
			}
			destroyCake(registry);
			destroyCharacter(registry);
			cur = clients.front();
			clients.pop();
			spawnNewCharacter(registry);
			ds->SetupDialogue(cur->start);
		}



	}
};