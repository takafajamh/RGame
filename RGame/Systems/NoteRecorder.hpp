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

	BeatNote m_currentNotes[4];

	const float SliderThreshhold = 0.3f;

public:
	NoteRecorder(Mix_Music* music)
	{
		m_music = music;
		m_currentNotes[0] = {};
		m_currentNotes[1] = {};
		m_currentNotes[2] = {};
		m_currentNotes[3] = {};
	}

	void Save(const std::string& path)
	{
		nlohmann::json j;
		j["notes"] = nlohmann::json::array();

		for (const auto& note : m_notes)
		{
			j["notes"].push_back({ { "time", note.time }, { "column", note.column }, { "length", note.length } });
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
					m_currentNotes[k.col] = { time, k.col, 0 };
					m_keyState[k.key] = true;
				}
			}
			else if(m_currentNotes[k.col].column != -1)
			{
				m_currentNotes[k.col].length = time - m_currentNotes[k.col].time;
				if (m_currentNotes[k.col].length < SliderThreshhold) m_currentNotes[k.col].length = 0;

				m_notes.push_back(m_currentNotes[k.col]);
				m_currentNotes[k.col] = { 0, -1, 0 };

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
