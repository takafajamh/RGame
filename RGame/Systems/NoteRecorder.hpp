#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>
#include <fstream>
#include "NoteSpawner.hpp"

class NoteRecorder : public ISystem
{
private:
	Mix_Music* m_music;
	std::vector<BeatNote> m_notes;
	bool m_lastF5 = false;
	bool m_keyState[256]{}; // simple debounce

public:
	NoteRecorder(Mix_Music* music)
	{
		m_music = music;
	}

	void Save(const std::string& path)
	{
		nlohmann::json j;
		j["notes"] = nlohmann::json::array();

		for (const auto& note : m_notes)
		{
			j["notes"].push_back({ { "time", note.time }, { "column", note.column } });
		}

		std::ofstream file(path);
		if (file.is_open())
			file << j.dump(4);
	}

	void Update(entt::registry& registry) override
	{
		const bool* keys = SDL_GetKeyboardState(nullptr);
		double time = Mix_GetMusicPosition(m_music);

		struct KeyColumn { SDL_Scancode key; int col; };
		KeyColumn keymap[] = {
			{ SDL_SCANCODE_LEFT, 0 },
			{ SDL_SCANCODE_UP, 1 },
			{ SDL_SCANCODE_DOWN, 2 },
			{ SDL_SCANCODE_RIGHT, 3 }
		};

		// Input recording
		for (auto& k : keymap)
		{
			if (keys[k.key])
			{
				if (!m_keyState[k.key]) // only once per press
				{
					m_notes.push_back({ time, k.col });
					m_keyState[k.key] = true;
				}
			}
			else
			{
				m_keyState[k.key] = false;
			}
		}

		// Save trigger
		if (keys[SDL_SCANCODE_F5])
		{
			if (!m_lastF5)
			{
				Save("Music/mapped.json");
				spdlog::info("Saved beatmap with {} notes", m_notes.size());
				m_lastF5 = true;
			}
		}
		else m_lastF5 = false;
	}
};
