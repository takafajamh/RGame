#pragma once
//./

#pragma once

#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <string>




class DialogueSystem : public ISystem
{
private:
	bool running = true;
	DialogueLine* cur = nullptr;

	entt::entity background = entt::null;
	std::vector<entt::entity> elements;
	std::shared_ptr<Font> m_font;
	std::shared_ptr<Texture> t_dialogueOption;

	bool clear = false;
	bool startUp = false;

public:
	bool flagChanged = false;

	DialogueSystem(std::shared_ptr<Font> font, std::shared_ptr<Texture> t_option)
	{
		m_font = font;
		t_dialogueOption = t_option;
		dod();
	}

	void spawnBackground(entt::registry& registry)
	{

	}

	void spawnText(entt::registry& registry)
	{
		spdlog::info("[DialogueSystem] Spawned text {}:\"{}\"", (cur->name), (cur->text));
		Position namePos = {1500, 200};
		Position textPos = {1400, 250};
		Position optionsBase = { 1400, 400 };


		// Name
		{
			entt::entity name = registry.create();
			registry.emplace<Position>(name, namePos);

			Text t;
			t.font = m_font;
			t.color = { 0, 0, 0, 255 };
			t.content = cur->name;
			t.fontSize = 48;
			t.xSize = 300;
			t.ySize = 100;
			registry.emplace<Text>(name, t);

			elements.push_back(name);
		}
		
		// Main
		{
			entt::entity content = registry.create();
			registry.emplace<Position>(content, textPos);

			Text t;
			t.font = m_font;
			t.color = { 0, 0, 0, 255 };
			t.content = cur->text;
			t.fontSize = 32;
			t.xSize = 800;
			t.ySize = 100;
			registry.emplace<Text>(content, t);

			elements.push_back(content);
		}

		// Options
		for (size_t i = 0; i < cur->options.size(); i++)
		{
			entt::entity op = registry.create();
			registry.emplace<Position>(op, Position{ optionsBase.x,optionsBase.y + (float)i * (60.f)});

			Text t;
			t.font = m_font;
			t.color = { 0, 0, 0, 255 };
			t.content = cur->options.at(i)->text;
			t.fontSize = 32;
			t.xSize = 450;
			t.ySize = 200;
			registry.emplace<Text>(op, t);

			elements.push_back(op);

			entt::entity but = registry.create();
			registry.emplace<Position>(but, Position{ optionsBase.x - 10, -6 + optionsBase.y + (float)i * (60.f) });
			Sprite s_but;
			s_but.sizeX = 400;
			s_but.sizeY = 50;
			s_but.texture = t_dialogueOption;
			s_but.useTextureRect = true;
			s_but.textureRect = { 0,0,300,50 };
			registry.emplace<Sprite>(but, s_but);

			TextureButton tb;
			tb.ClickRect = { 0, 0, 300, 50 };
			tb.HoverRect = { 0, 50, 300, 50 };
			registry.emplace<TextureButton>(but, tb);

			OptionEffector oe;
			oe.cur = cur;
			oe.optionId = i;
			oe.flagChanged = &flagChanged;
			registry.emplace<OptionEffector>(but, oe);


			elements.push_back(but);

		}

	}

	void resetText(entt::registry& registry)
	{
		if (!registry.valid(background))
		{
			spawnBackground(registry);
		}

		for (entt::entity& e : elements)
		{
			registry.destroy(e);
		}
		elements.clear();
	
		spawnText(registry);
	}

	void dissapear(entt::registry& registry)
	{
		if(registry.valid(background))
			registry.destroy(background);

		for (entt::entity& e : elements)
		{
			registry.destroy(e);
		}
		elements.clear();
	}

	void Update(entt::registry& registry)
	{
		if (!running || cur == nullptr)
			return;

		if (!startUp)
		{
			resetText(registry);
			startUp = true;
		}
		if (flagChanged)
		{
			cur = cur->chosenOption;
			resetText(registry);
			flagChanged = false;
		}

		float mouseX, mouseY;
		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		bool clicked = mouseState & SDL_BUTTON_LMASK;
		if (clicked && clear)
		{
			if (cur->options.size() == 0)
			{
				clear = false;
				cur = cur->next;
				if (cur == nullptr)
					dissapear(registry);
				else
					resetText(registry);

			}
			else
			{
				// handle options-
			}
			
		}
		else if(!clicked)
		{
			clear = true;
		}


	}

	DialogueLine* CreateDialogueList(const std::vector<DialogueLine*>& disconnected)
	{
		if (disconnected.size() == 0)
			return nullptr;

		for (size_t i = 0; i < disconnected.size()-1; i++)
		{
			disconnected.at(i)->next = disconnected.at(i + 1);
		}
		return disconnected.at(0);
	}

	void dod()
	{
		DialogueLine* dl1 = new DialogueLine("Miss Manager", "Are you ready for your first day?");

		DialogueLine* dl4 = new DialogueLine("You", "I guess I am alone now...");
		DialogueLine* dl5 = new DialogueLine("You", "Let's look around before any customer comes...");
		DialogueLine* dl45 = CreateDialogueList({ dl4, dl5 });


		DialogueLine* dl20 = new DialogueLine("You", "Yes?");
		DialogueLine* dl21 = new DialogueLine("Miss Manager", "Awesome, you will get everything when you see it, good luck!");
		DialogueLine* dl22 = new DialogueLine("You", "Wait-");
		DialogueLine* dl2 = CreateDialogueList({ dl20, dl21, dl22, dl4 });

		DialogueLine* dl30 = new DialogueLine("You", "No?");
		DialogueLine* dl31 = new DialogueLine("Miss Manager", "Quite a shame! I've already signed a contract with you, so you have no choice");
		DialogueLine* dl32 = new DialogueLine("You", "I guess-");
		DialogueLine* dl33 = new DialogueLine("Miss Manager", "Well see you then!");
		DialogueLine* dl3 = CreateDialogueList({ dl30, dl31, dl32, dl33, dl4 });

		dl1->options.push_back(dl2);
		dl1->options.push_back(dl3);

		cur = dl1;
		startUp = false;
	}


};