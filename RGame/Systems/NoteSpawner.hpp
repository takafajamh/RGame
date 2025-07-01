#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <KitsuEngine/KitsuneEngine.hpp>

struct BeatNote
{
	double time;
	int column;
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
			out.push_back({ time, column });
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

	void SpawnNote(entt::registry& registry, int tnum, float carry = 0)
	{
		constexpr int vals[4] = { 0, 3, 1, 2 }; // your custom mapping
		const int num = vals[tnum];

		entt::entity arrow = registry.create();
		ScreenPosition& sp = registry.emplace<ScreenPosition>(arrow, ScreenPosition{ sPos + (float)(num * m), -50 });

		Sprite sArrow;
		sArrow.sizeX = 16 * 4;
		sArrow.sizeY = 16 * 4;
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
		registry.emplace<Note>(arrow, Note{ num });
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
			SpawnNote(registry, note.column, carry);

			++m_nextNoteIndex;
		}
	}
};
