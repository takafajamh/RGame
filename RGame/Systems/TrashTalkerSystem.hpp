#pragma once
#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <string>
#include <vector>

class TrashTalkerSystem : public ISystem
{
public:

	std::vector<std::string> trash = {
	"They will surely die, won't they? ",
	"Look at them, love is blooming around us, eh, in my times... ",
	"They will fuck, trust me. ",
	"All on red. ",
	"Man, get outta my field. ",
	"Makes me go back... ",
	"Now that's a good one! ",
	"Oh, a horrible matchup, really horrible... ",
	"I feel lucky! ",
	"*Humming* ",
	"Dead, dead... mmm... Dead. ",
	"That might just go well... ",
	"Well that one surely goes horribly wrong... ",
	"Ahaha! ",
	"Aha haha! ",
	"Gyahaha! ",
	"Muahaha! ",
	"Terribly entertaining...  ",
	"I win this one, friends. ",
	"There is no beating this! ",
	"They just can't loose this one! ",
	"Wish I could've caught that quest... ",
	"Makes you think of the good old days... ",
	"Inconceivable! ",
	"Oh! ... No, wait, I forgot. ",
	"Hm? Oh, yes. ",
	"Oh no, oh no no no no no... ",
	"Takes me back... ",
	"I wish I was there with them... not! ",
	"I wish I was there with them... ",
	"I'm buying a round with the winnings form this one. ",
	"What are you, stupid? There's no way! ",
	"Sometimes I think that axe in your head isn't just for show! ",
	"At times like these I think that arrow hit your head - not your knee! ",
	"I was an adventurer like th- well, I know you know the story, but I like to tell it! ",
	"No way. ",
	"Nuh-uh. ",
	"Oh yes! ",
	"Hell yes! ",
	"Hell yeah! ",
	"Sometimes my genius... it's almost frightening... ",
	"Sometimes your stupidity is astonishing! ",
	"Sometimes their stupidity really amazes me... ",
	"How do you even get to that situation? ",
	"How do you even find a team like that? ",
	"How does a party like that even form? ",
	"No way that's real... ",
	"Must be a scam, I'm telling you! ",
	"That's no quest! ",
	"That's no party! ",
	"That reminds me of that one time... ",
	"That's the last one, then I have to go... ",
	"No, really, that's the last one! ",
	"Well now it really is the last one... ",
	"Sometimes I wonder, if what we're doing is moral... ",
	"Sometimes I wonder about how they're doing... ",
	"Sometimes I wish my lover wasn't eaten by that Tarrasque... ",
	"Sometimes I wish she didn't turn into a vampire... ",
	"Sometimes I wish he didn't turn out to be a demon... ",
	"Why did you even?- Ah, no matter. ",
	"Free win for me! ",
	"Free drinks for me! ",
	"Losers, the lot'a ya! ",
	"You're so daft it's wild! ",
	"Are you sure you ain't undead? ",
	"Seems like there's no brain in that head of yours... ",
	"A-a-a, made you look! ",
	"Good thing they don't have to find the Mind Goblin... ",
	"We should invite Joe to those games of ours! ",
	"You mother fucker. ",
	"Your mom. ",
	"That one I know! ",
	"Maybe if they turn into weights we could fight them... ",
	"Oi, no rabblerousing in here! ",
	"Gay. All of them. For sure. ",	
	
	};

	void Update(entt::registry& registry) override
	{
		auto view = registry.view<TrashTalker, Text>();

		for (auto [entity, tt, txt] : view.each())
		{
			if (tt.dTime > 0)
			{
				tt.dTime -= dt;
				txt.content = "";
				continue;
			}

			tt.time += dt;
			if (tt.time >= tt.maxTime)
			{
				tt.text = trash.at(std::rand() % trash.size());
				tt.time = 0;
				tt.maxTime = tt.text.size() * 0.04f + 2;
				tt.dTime = ((float)(std::rand() % 500) / (float)100);
			}
			else
			{
				// how many chars should be visible
				float progress = tt.time / (tt.maxTime - 2);
				std::size_t charsToShow = static_cast<std::size_t>(progress * tt.text.size());

				if (charsToShow > tt.text.size())
					charsToShow = tt.text.size();

				// update displayed text
				txt.content = tt.text.substr(0, charsToShow);
			}
		}
	}
};