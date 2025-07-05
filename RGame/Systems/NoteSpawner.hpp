#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>

struct BeatNote
{
	double time;
	int column;
	float length = 0;
};

bool LoadBeatmapJson(const std::string& path, std::vector<BeatNote>& out)
{
	using json = nlohmann::json;
	std::ifstream file(path);
	if (!file.is_open()) return false;

	json j;
	file >> j;

	if (!j.contains("notes") || !j["notes"].is_array()) return false;

	for (const auto& note : j["notes"]) 
	{
		if (note.contains("time") && note.contains("column")) 
		{
			double time = note["time"];
			int column = note["column"];
			out.push_back({ time, column, 0 });
		}

		if (note.contains("length"))
		{
			out.at(out.size() - 1).length = note["length"];
		}
	}

	std::sort(out.begin(), out.end(), [](auto& a, auto& b) 
		{
		return a.time < b.time;
		});

	return true;
}


class NoteSpawner : public ISystem
{
private:
	double m_fallTime = 2.0;

	std::vector<BeatNote> m_beatmap;
	size_t m_nextNoteIndex = 0;

	Mix_Music* Music;
	std::shared_ptr<Texture> t_Arrows;

	float m = 100;
	float sPos = 660;

public:
	NoteSpawner(Mix_Music* music, std::shared_ptr<Texture> t_arrows, const std::vector<BeatNote>& beatmap)
		: Music(music), t_Arrows(t_arrows), m_beatmap(beatmap)
	{
	}

	entt::entity SpawnNote(entt::registry& registry, int tnum, float carry = 0, bool endNote = false, float length = 0)
	{
		constexpr int vals[4] = { 0, 3, 1, 2 }; // your custom mapping
		const int num = vals[tnum];
		constexpr int noteSize = 16 * 4;

		entt::entity arrow = registry.create();

		ScreenPosition& sp = registry.emplace<ScreenPosition>(arrow, ScreenPosition{ sPos + (float)(num * m), -50 });
		sp.y += length;

		Sprite sArrow;
		sArrow.sizeX = noteSize;
		sArrow.sizeY = noteSize;
		sArrow.texture = t_Arrows;
		sArrow.useTextureRect = true;
		sArrow.layerOrder = 10;
		sArrow.textureRect = { 16 * 2, 16 * (float)tnum, 16, 16 };
		registry.emplace<Sprite>(arrow, sArrow);

		Mover m;
		m.Speed = (595 + 50) / m_fallTime;
		registry.emplace<Mover>(arrow, m);

		sp.y += carry * m.Speed;

		registry.emplace<RemoveAfterDelay>(arrow, RemoveAfterDelay{ 5, 0 });

		if(!endNote) registry.emplace<Note>(arrow, Note{ num });
		else registry.emplace<EndNote>(arrow, EndNote{ num });

		return arrow;
	}

	void SpawnLongNote(entt::registry& registry, int tnum, float noteTime, float carry = 0)
	{
		constexpr int noteSize = 16 * 4;
		constexpr int vals[4] = { 0, 3, 1, 2 }; // your custom mapping
		const int num = vals[tnum];

		entt::entity arrow = registry.create();
		ScreenPosition& sp = registry.emplace<ScreenPosition>(arrow, ScreenPosition{ sPos + (float)(num * m), -50 });

		RectangleShape sArrow;
		sArrow.width = 16 * 4;
		sArrow.layer = 9;
		sArrow.color = { 130,130,130,130 };

		Mover m;
		m.Speed = (595 + 50) / m_fallTime;
		registry.emplace<Mover>(arrow, m);

		sp.y += carry * m.Speed;

		sArrow.height = noteTime * m.Speed;

		registry.emplace<RemoveAfterDelay>(arrow, RemoveAfterDelay{ 5, 0 });

		entt::entity startNote = SpawnNote(registry, tnum, carry, false, sArrow.height - noteSize);
		entt::entity endNote = SpawnNote(registry, tnum, carry, true);

		registry.emplace<LongNote>(arrow, LongNote{ startNote, endNote, num, noteTime, false });
		registry.emplace<RectangleShape>(arrow, sArrow);
	}

	void Update(entt::registry& registry) override
	{
		if (m_nextNoteIndex >= m_beatmap.size())
			return;

		double currentTime = Mix_GetMusicPosition(Music);

		// We want to spawn the note early so it arrives just in time
		double spawnThreshold = currentTime + m_fallTime;

		while (m_nextNoteIndex < m_beatmap.size() && m_beatmap[m_nextNoteIndex].time <= spawnThreshold)
		{
			const BeatNote& note = m_beatmap[m_nextNoteIndex];

			// Carry is how much "late" we are in spawning it (small error correction)
			float carry = std::max(0.0, spawnThreshold - note.time);
			if (note.length == 0)
				SpawnNote(registry, note.column, carry);
			else
				SpawnLongNote(registry, note.column, note.length, carry);

			++m_nextNoteIndex;
		}
	}
};
