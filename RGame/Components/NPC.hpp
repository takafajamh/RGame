#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <map> 
#include <fstream>
#include <nlohmann/json.hpp>

using DialogueFlags = uint8_t;

enum class DialogueFlag : uint8_t
{
	None = 0,
	TalkedWithA = 1 << 0,		// 000001
	TalkedWithB = 1 << 1,		// 000010
	TalkedWithC = 1 << 2,		// 000100
	TalkedWithD = 1 << 3,		// 001000
	TalkedWithE = 1 << 4,		// 010000
	TalkedWithF = 1 << 5,		// 100000
};

inline DialogueFlags operator|(DialogueFlag a, DialogueFlag b)
{
	return static_cast<DialogueFlags>(a) | static_cast<DialogueFlags>(b);
}

inline DialogueFlags& operator|=(DialogueFlags& flags, DialogueFlag flag)
{
	flags = static_cast<DialogueFlags>(flags | static_cast<DialogueFlags>(flag));
	return flags;
}

inline bool HasFlag(DialogueFlags flags, DialogueFlag flag)
{
	return (flags & static_cast<DialogueFlags>(flag)) != 0;
}


struct DialogueLine
{
	std::string Name;
	std::string Text;
};

/// <summary>
/// Required for:
/// Dress: 0,1,2
/// Time = 0,1,2,3,4,5,6,7
/// NumberofConversations 0 
/// 
/// So we need at least 24 dialogues per NPC
/// Ideally 48+ (Case Talk second time)
/// 
/// + Flags, standard = 0
/// </summary>
struct DialogueContext
{
	int Dress = 0; // 0 - 2
	int Time = 0; // 0 - 7
	int NumberOfConversarions = 0;
	DialogueFlags Flags = 0; 

	bool operator==(const DialogueContext& other)
	{
		return Dress == other.Dress &&
			Time == other.Time &&
			NumberOfConversarions == other.NumberOfConversarions &&
			Flags == other.Flags;
	}

};

struct DialogueContextKey
{
	int Dress;
	int Time;
	DialogueFlags Flags;

	bool operator==(const DialogueContextKey& other) const
	{
		return Dress == other.Dress && Time == other.Time && Flags == other.Flags;
	}

	bool operator<(const DialogueContextKey& other) const
	{
		return std::tie(Dress, Time, Flags) < std::tie(other.Dress, other.Time, other.Flags);
	}
};

// hash
namespace std
{
	template <>
	struct hash<DialogueContextKey>
	{
		std::size_t operator()(const DialogueContextKey& ctx) const
		{
			std::size_t h = std::hash<int>()(ctx.Dress);
			h ^= (std::hash<int>()(ctx.Time) << 1);
			h ^= (std::hash<DialogueFlags>()(ctx.Flags) << 2);
			return h;
		}
	};
};

struct NPC
{
	std::unordered_map<DialogueContextKey, std::map<int, std::vector<DialogueLine>>> Dialogues;
};

void SetDialogue(NPC& npc, const std::vector<DialogueLine>& lines, const DialogueContext& ctx)
{
	DialogueContextKey key{ ctx.Dress, ctx.Time, ctx.Flags };
	npc.Dialogues[key][ctx.NumberOfConversarions] = lines;
}

const std::vector<DialogueLine>& GetDialogue(const NPC& npc, const DialogueContext& ctx)
{
	DialogueContextKey key{ ctx.Dress, ctx.Time, ctx.Flags };

	auto outerIt = npc.Dialogues.find(key);
	if (outerIt == npc.Dialogues.end())
	{
		spdlog::error("No dialogues found for Dress={}, Time={}", ctx.Dress, ctx.Time);
		static const std::vector<DialogueLine> empty;
		return empty;
	}

	const auto& convMap = outerIt->second;
	auto convIt = convMap.upper_bound(ctx.NumberOfConversarions);

	if (convIt == convMap.begin())
	{
		spdlog::error("No dialogues for conversations <= {} for Dress={}, Time={}, Flags={}",
			ctx.NumberOfConversarions, ctx.Dress, ctx.Time, static_cast<int>(ctx.Flags));
		static const std::vector<DialogueLine> empty;
		return empty;
	}

	--convIt; // fallback to largest conversation number <= requested
	return convIt->second;
}

NPC& LoadNPCFromJSON(const std::string& path, NPC& toLoad)
{
	using json = nlohmann::json;

	std::ifstream in(path);
	if (!in)
	{
		spdlog::error("Failed to open NPC JSON file: {}", path);
		return toLoad;
	}

	json j;
	in >> j;

	int totalEntries = 0;
	int emptyLinesCount = 0;

	for (const auto& entry : j)
	{
		DialogueContext ctx;
		ctx.Dress = entry.at("Dress").get<int>();
		ctx.Time = entry.at("Time").get<int>();
		ctx.NumberOfConversarions = entry.at("NumberOfConversations").get<int>();

		// Handle Flags
		ctx.Flags = 0;
		if (entry.contains("Flags"))
		{
			for (const auto& flagStr : entry["Flags"])
			{
				std::string flag = flagStr.get<std::string>();

				if (flag == "TalkedWithA") ctx.Flags |= DialogueFlag::TalkedWithA;
				else if (flag == "TalkedWithB") ctx.Flags |= DialogueFlag::TalkedWithB;
				else if (flag == "TalkedWithC") ctx.Flags |= DialogueFlag::TalkedWithC;
				else if (flag == "TalkedWithD") ctx.Flags |= DialogueFlag::TalkedWithD;
				else if (flag == "TalkedWithE") ctx.Flags |= DialogueFlag::TalkedWithE;
				else if (flag == "TalkedWithF") ctx.Flags |= DialogueFlag::TalkedWithF;
				else {
					spdlog::warn("Unknown flag '{}' in {} (Dress={}, Time={}, Conv={})",
						flagStr.get<std::string>(), path,
						ctx.Dress, ctx.Time, ctx.NumberOfConversarions);
				}
			}
		}

		// Parse Lines
		std::vector<DialogueLine> lines;
		for (const auto& line : entry.at("Lines"))
		{
			DialogueLine dl{
				line.at("Name").get<std::string>(),
				line.at("Text").get<std::string>()
			};
			if (dl.Text.empty()) emptyLinesCount++;
			lines.push_back(std::move(dl));
		}

		SetDialogue(toLoad, lines, ctx);
	}

	spdlog::info("Finished loading NPC from '{}': {} entries, {} empty dialogue lines",
		path, totalEntries, emptyLinesCount);

	return toLoad;
}

