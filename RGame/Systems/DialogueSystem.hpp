#pragma once
//./

#pragma once

#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <string>


struct DialogueLine
{
	std::string text;
	std::string name;
	DialogueLine* next = nullptr;

	std::vector<DialogueLine*> options;

	DialogueLine(){}
	DialogueLine(const std::string& nm,const std::string& txt)
	{
		text = txt;
		name = nm;
	}

};

class DialogueSystem : public ISystem
{
private:
	bool running = true;
	DialogueLine* cur = nullptr;

	entt::entity background = entt::null;
	std::vector<entt::entity> elements;
	std::shared_ptr<Font> m_font;

public:
	DialogueSystem(std::shared_ptr<Font> font)
	{
		m_font = font;
	}

	void spawnBackground(entt::registry& registry)
	{

	}

	void spawnText(entt::registry& registry)
	{

		// Name
		{
			entt::entity name = registry.create();
			registry.emplace<Position>(name, Position{ 1200,400 });

			Text t;
			t.font = m_font;
			t.color = { 0, 0, 0, 255 };
			t.content = cur->name;
			t.fontSize = 48;
			t.xSize = 400;
			t.ySize = 200;
			registry.emplace<Text>(name, t);

			elements.push_back(name);
		}
		
		// Main
		{
			entt::entity content = registry.create();
			registry.emplace<Position>(content, Position{ 1000,500 });

			Text t;
			t.font = m_font;
			t.color = { 0, 0, 0, 255 };
			t.content = cur->text;
			t.fontSize = 32;
			t.xSize = 400;
			t.ySize = 200;
			registry.emplace<Text>(content, t);

			elements.push_back(content);
		}

		// Options
		for (size_t i = 0; i < cur->options.size(); i++)
		{
			entt::entity op = registry.create();
			registry.emplace<Position>(op, Position{ 1000,650 + (float)i * (50.f)});

			Text t;
			t.font = m_font;
			t.color = { 0, 0, 0, 255 };
			t.content = cur->text;
			t.fontSize = 32;
			t.xSize = 400;
			t.ySize = 200;
			registry.emplace<Text>(op, t);

			elements.push_back(op);
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


		bool clicked = false;
		if (clicked)
		{
			cur = cur->next;
			if (cur == nullptr)
				dissapear(registry);
			else
				resetText(registry);
		}


	}

	DialogueLine* CreateDialogueList(const std::vector<DialogueLine*>& disconnected)
	{
		for (size_t i = 0; i < disconnected.size()-1; i++)
		{
			disconnected.at(i)->next = disconnected.at(i + 1);
		}
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

	}


};