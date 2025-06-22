#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>



// --- Flag Definitions (Unchanged) ---
using DialogueFlags = uint8_t;
enum class DialogueFlag : uint8_t
{
	None = 0,
	fucked_May = 1 << 0,	// 000001
	talked_Carl = 1 << 1,	// 000010
	drink = 1 << 2,			// 000100
	talked_MsP = 1 << 3,	// 001000
	talked_Ted = 1 << 4,	// 010000
	TalkedWithF = 1 << 5,	// 100000
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



// Helper to convert string to flag
DialogueFlag StringToFlag(const std::string& flagStr) 
{
	if (flagStr == "fucked_May") return DialogueFlag::fucked_May;
	if (flagStr == "talked_Carl") return DialogueFlag::talked_Carl;
	if (flagStr == "drink") return DialogueFlag::drink;
	if (flagStr == "talked_MsP") return DialogueFlag::talked_MsP;
	if (flagStr == "talked_Ted") return DialogueFlag::talked_Ted;
	if (flagStr == "TalkedWithF") return DialogueFlag::TalkedWithF;
	
	return DialogueFlag::None;
}


struct DialogueLine
{
	std::string Name;
	std::string Text;

	DialogueLine(std::string name, std::string text)
	{
		Name = name;
		Text = text;
	}
};

// Represents the current state of the game world to check against
struct DialogueContext
{
	int Dress = 0; // 0 - 2
	int Time = 0; // 0 - 7
	int NumberOfConversations = 0;
	DialogueFlags Flags = 0;
};

// Represents a single dialogue block from the JSON file
struct DialogueEntry
{
	// Conditions to trigger this dialogue
	int Dress;
	int Time;
	int NumberOfConversations;
	DialogueFlags RequiredFlags = 0;

	// Metadata
	int Priority = 0;
	DialogueFlags FlagsToSet = 0; // For "SetFlags"

	// Content
	std::vector<DialogueLine> Lines;
};

struct NPC
{
	std::vector<DialogueEntry> AllDialogues;
	std::vector<Position> positions;
	int talks = 0;
};


/// <summary>
/// Finds the best matching dialogue entry based on the current game context.
/// </summary>
/// <param name="npc">The NPC to get dialogue from.</param>
/// <param name="ctx">The current game context (player's state).</param>
/// <returns>
/// A constant pointer to the best matching DialogueEntry, or nullptr if no suitable dialogue is found.
/// </returns>
/// <remarks>
/// This function iterates through all possible dialogues for an NPC and finds the
/// best match according to a clear hierarchy of rules:
/// <para>
/// 1.  <b>Filtering:</b> It first gathers all dialogue entries that are "valid" for the
///     current context. A valid entry must satisfy all of its conditions:
///     <list type="bullet">
///         <item><description>Wildcards (-1) in an entry's conditions always match the context.</description></item>
///         <item><description>Specific values (Dress, Time) must match the context exactly.</description></item>
///         <item><description>All `RequiredFlags` must be present in the context's `Flags`.</description></item>
///         <item><description>The entry's `NumberOfConversations` must be less than or equal to the context's current count (or be a wildcard).</description></item>
///     </list>
/// </para>
/// <para>
/// 2.  <b>Selection:</b> From the list of valid entries, it selects the best one:
///     <list type="number">
///         <item><description><b>Highest Priority Wins:</b> The entry with the highest `Priority` value is chosen.</description></item>
///         <item><description><b>Most Specific Match Wins (Tie-Breaker):</b> If multiple entries share the highest priority, the one with the highest `NumberOfConversations` is chosen, as it is considered more specific to the current state.</description></item>
///     </list>
/// </para>
/// This logic correctly handles wildcards and fallbacks, ensuring the most relevant dialogue is always selected.
/// </remarks>
const DialogueEntry* GetDialogue(const NPC& npc, const DialogueContext& ctx)
{
	const DialogueEntry* bestMatch = nullptr;

	// We store the "score" of the best match found so far to compare against.
	int bestPriority = -1;
	int bestConvNumber = -1; // Used for tie-breaking

	for (const auto& entry : npc.AllDialogues)
	{
		// --- Phase 1: Filtering ---
		// If this entry isn't a valid candidate, skip it immediately.

		// Condition: Dress
		if (entry.Dress != -1 && entry.Dress != ctx.Dress) 
		{
			continue;
		}

		// Condition: Time
		if (entry.Time != -1 && entry.Time != ctx.Time)
		{
			continue;
		}

		// Condition: Required Flags (all must be present)
		if ((ctx.Flags & entry.RequiredFlags) != entry.RequiredFlags)
		{
			continue;
		}

		// Condition: NumberOfConversations
		// The entry's conversation count must be a wildcard OR less than/equal to the context's.
		if (entry.NumberOfConversations != -1 && entry.NumberOfConversations > ctx.NumberOfConversations) 
		{
			continue;
		}

		// --- Phase 2: Selection ---
		// This entry is a valid candidate. Now, is it better than the current bestMatch?

		if (bestMatch == nullptr) 
		{
			// This is the first valid match we've found.
			bestMatch = &entry;
			bestPriority = entry.Priority;
			bestConvNumber = entry.NumberOfConversations;
		}
		else if (entry.Priority > bestPriority) 
		{
			// Rule A: This entry has a higher priority, so it's the new best match.
			bestMatch = &entry;
			bestPriority = entry.Priority;
			bestConvNumber = entry.NumberOfConversations;
		}
		else if (entry.Priority == bestPriority) 
		{
			// Rule B (Tie-Breaker): Priorities are the same. Check for specificity.
			// A higher conversation number is more specific. This replicates your original logic.
			if (entry.NumberOfConversations > bestConvNumber) {
				bestMatch = &entry;
				// bestPriority remains the same
				bestConvNumber = entry.NumberOfConversations;
			}
		}
	}

	if (!bestMatch) 
	{
		spdlog::warn("No suitable dialogue found for NPC with context: Dress={}, Time={}, Convs={}, Flags={}",
			ctx.Dress, ctx.Time, ctx.NumberOfConversations, static_cast<int>(ctx.Flags));
	}

	return bestMatch;
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

	toLoad.AllDialogues.clear();

	for (const auto& j_entry : j)
	{
		DialogueEntry currentEntry;

		// Get conditions, with defaults for wildcard (-1)
		currentEntry.Dress = j_entry.value("Dress", -1);
		currentEntry.Time = j_entry.value("Time", -1);
		currentEntry.NumberOfConversations = j_entry.value("NumberOfConversations", -1);
		currentEntry.Priority = j_entry.value("Priotity", 0); // Default priority is 0

		// Handle "Flags" (requirements)
		if (j_entry.contains("Flags"))
		{
			for (const auto& flagStr : j_entry["Flags"])
			{
				DialogueFlag flag = StringToFlag(flagStr.get<std::string>());
				if (flag != DialogueFlag::None) 
				{
					currentEntry.RequiredFlags |= flag;
				}
				else 
				{
					spdlog::warn("Unknown required flag '{}' in {}", flagStr.get<std::string>(), path);
				}
			}
		}

		// Handle "SetFlags" (to be applied after dialogue)
		if (j_entry.contains("SetFlags"))
		{
			// Support both a single string and an array of strings for flexibility
			if (j_entry["SetFlags"].is_string()) 
			{
				DialogueFlag flag = StringToFlag(j_entry["SetFlags"].get<std::string>());
				if (flag != DialogueFlag::None) currentEntry.FlagsToSet |= flag;
			}
			else if (j_entry["SetFlags"].is_array()) 
			{
				for (const auto& flagStr : j_entry["SetFlags"])
				{
					DialogueFlag flag = StringToFlag(flagStr.get<std::string>());
					if (flag != DialogueFlag::None) 
					{
						currentEntry.FlagsToSet |= flag;
					}
					else 
					{
						spdlog::warn("Unknown flag to set '{}' in {}", flagStr.get<std::string>(), path);
					}
				}
			}
		}

		// Parse "Lines"
		if (j_entry.contains("Lines"))
		{
			for (const auto& line : j_entry["Lines"])
			{
				currentEntry.Lines.emplace_back(
					line.at("Name").get<std::string>(),
					line.at("Text").get<std::string>()
				);
			}
		}

		toLoad.AllDialogues.push_back(std::move(currentEntry));
	}

	// Optional but recommended: Sort by priority descending.
	// This can make the tie-breaking logic in GetDialogue slightly more straightforward
	// if you ever change it, and ensures a stable order for equal-priority items.
	std::sort(toLoad.AllDialogues.begin(), toLoad.AllDialogues.end(),
		[](const DialogueEntry& a, const DialogueEntry& b) {
			return a.Priority > b.Priority;
		});

	spdlog::info("Finished loading NPC from '{}': {} dialogue entries loaded.", path, toLoad.AllDialogues.size());

	return toLoad;
}