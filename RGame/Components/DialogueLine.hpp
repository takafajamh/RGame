#pragma once
#include <string>
#include <vector>


struct DialogueLine
{
	std::string text;
	std::string name;
	DialogueLine* next = nullptr;
	DialogueLine* chosenOption = nullptr;

	std::vector<DialogueLine*> options;

	DialogueLine() {}
	DialogueLine(const std::string& nm, const std::string& txt)
	{
		text = txt;
		name = nm;
	}

};