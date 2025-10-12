#pragma once

#include "../Components.hpp"
#include <KitsuEngine/System.hpp>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <random>
/*
// Does not account for just position, uses Screen Position

class TokenSystem : public ISystem
{
private:


	struct result
	{
		bool die;
		bool dieOne;
		bool survive;
		bool fail;
		bool lvlUp;
		bool sucess;
		bool trauma;
		bool love;
	};

	Uint32 prevMouseState = 0;
	bool m_holding = false;
	Game* m_game = nullptr;

	float delay = -1;

	bool intersects(float x1, float y1, float w1, float h1,
		float x2, float y2, float w2, float h2) 
	{
		return !(x1 + w1 < x2 ||
			x2 + w2 < x1 ||
			y1 + h1 < y2 ||
			y2 + h2 < y1);
	}

	bool chance(int percentage) 
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(1, 100);

		return dist(gen) <= percentage;
	}

	int checkCollision(entt::registry& registry, entt::entity entity)
	{
		auto& pos1 = registry.get<ScreenPosition>(entity);
		auto& spr1 = registry.get<Sprite>(entity);

		for (size_t i = 0; i < buttons.size(); i++) 
		{
			entt::entity other = buttons[i];

			auto& pos2 = registry.get<ScreenPosition>(other);
			auto& spr2 = registry.get<Sprite>(other);

			if (intersects(pos1.x, pos1.y, spr1.sizeX, spr1.sizeY,
				pos2.x, pos2.y, spr2.sizeX, spr2.sizeY)) {
				return static_cast<int>(i);
			}
		}
		return -1; // no collision
	}

	void buttonUpdate(entt::registry& registry)
	{
		float mouseX, mouseY;
		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		auto view = registry.view<TokenComp, Sprite, ScreenPosition>();

		for (auto [entity, token, sprite, pos] : view.each())
		{
			SDL_FRect rect =
			{
				pos.x,
				pos.y,
				sprite.sizeX,
				sprite.sizeY
			};

			bool hovered =
				mouseX >= rect.x && mouseX <= rect.x + rect.w &&
				mouseY >= rect.y && mouseY <= rect.y + rect.h;

			// Set current color based on state
			if (hovered)
			{
				if (mouseState & SDL_BUTTON_LMASK)
				{
					if (!m_holding)
					{
						m_holding = true;
						token.Holding = true;
					}
					
				}
				else
				{
					token.Holding = false;
					m_holding = false;
					token.BetState = checkCollision(registry, entity);
				}
			}
			else
			{
				token.Holding = false;
				token.BetState = checkCollision(registry, entity);
			}

		}
	}

	void settlePos(entt::registry& registry)
	{
		float mouseX, mouseY;
		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		auto view = registry.view<TokenComp, Sprite, ScreenPosition>();

		for (auto [entity, token, sprite, pos] : view.each())
		{
			if (token.Holding)
			{
				pos.x = mouseX - (sprite.sizeX / 2);
				pos.y = mouseY - (sprite.sizeY / 2);
			}
		}
	}

public:

	std::string replaceCharacters(const std::string& text, const std::vector<std::string>& names)
	{
		std::regex pattern(R"(\[chr(\d+)\])"); // matches [chr1], [chr2], etc.

		std::string result;
		std::sregex_iterator it(text.begin(), text.end(), pattern);
		std::sregex_iterator end;

		std::size_t last_pos = 0;
		for (; it != end; ++it)
		{
			std::smatch match = *it;
			std::size_t match_pos = match.position(0);      // position in original 'text'
			std::size_t match_len = match.length(0);

			// append text between last match and this match
			if (match_pos > last_pos)
				result.append(text, last_pos, match_pos - last_pos);

			// compute replacement
			int index = std::stoi(match[1]) - 1; // chr1 -> 0
			if (index >= 0 && index < static_cast<int>(names.size()))
			{
				result.append(names[index]);
			}
			else
			{
				// out of range: keep original placeholder (or you can append an empty string)
				result.append(match.str(0));
			}

			// move last_pos past the matched placeholder
			last_pos = match_pos + match_len;
		}

		// append remaining tail (or whole text if no matches)
		result.append(text, last_pos, std::string::npos);

		return result;
	}


	struct Q
	{
		std::string s;
		int dif = 1;
	};
	struct PM 
	{
		std::string t;
		std::string s;
		int dif = 2;
	};
	std::vector<PM> PartyMembers = { 
		{"Introspective Individuals ", "Human Wizard: [chr1] \nElf Wizard: [chr2] \nHalf-Orc Wizard: [chr3]",1},
		{"Party", "Human Fighter: [chr1] \nElf Wizard: [chr2] \nDwarf Barbarian: [chr3]", 1},
		{"The Bloody Edgedarkners", "Human Rouge: [chr1] \nTiefling Rouge: [chr2] \nElf Cleric: [chr3]", 1},
		{"The Friendship League", "Elf Wizard: [chr1] \nHalf-elf Sorcerer: [chr2] \nHuman Ranger: [chr3] \nTielfing Barbian: [chr4] \nGnome Bard: [chr5] \nHalf-elf Warlock: [chr6]", 1},

		{"Fighter Crew", "Human Fighter: [chr1] \nHuman Fighter: [chr2] \nHuman Fighter: [chr3]", 2},
		{"Party", "Human Fighter: [chr1] \nTiefling Warlock: [chr2] \nHalfling Bard: [chr3] \nElf cleric: [chr4]", 2},
		{"Bits and Bob", "Dwarf Artificer: [chr1] \nDwarf Fighter: [chr2] \nHuman Artificer: Bob", 2},
		{"Sparkling Conflagration", "Treefolk bard: [chr1] \nHuman bard: [chr2] \nDwarf artificer/bard: [chr3]", 2},
		{"The Golden Dragon's Scions", "Half-elf Sorcerer: [chr1] \nDwarf Cleric: [chr2] \nDragonborn Barbarian: [chr3] \nGnome Monk: [chr4]", 2},
		{"Real human party", "Three goblins in a trenchcoat: [chr1], [chr2] and [chr3] \nAn elf who always wears a hat: [chr4] \nA halfling on stilts: [chr5]", 2},

		{"WAAAAGH!!!", "Orc Barbarian: [chr1] \nOrc Fighter: [chr2] \nHalf-Orc Palladin: [chr3] \nOrc Fighter: [chr4]", 3},
		{"The Dawnbringers", "Kalashtar Warlock: Mhuirnin \nAasimar Wizard: Antrakt \nMimi Sorcerer: Eleiran \nHarengon Druid: Markiz \nDrakon Paladin: Ywain", 3},
		{"The Escaped Thralls", "Tiefling Barbarian \nGithyanki Fighter \nHalf-elf Cleric \nElf Rouge \nHuman Warlock \nHuman Wizard", 3},
		{"The Journey's End", "Human Paladin: [chr1] \nElf Wizard: [chr2] \nDwarf Fighter: [chr3] \nHuman Cleric: [chr4]", 3},


	};

	std::vector<entt::entity> buttons;
	TokenSystem(Game* game = nullptr)
	{
		m_game = game;
	}
	
	std::vector<std::string> Characters = {
"Sakuya Kagaya",
"Hibiki Higuchi",
"Akichi Kojirou",
"Katsu Sasaki",
"Erica Grenadier",
"Aru Jikogoto",
"Riku Oka",
"Jiro \"Shi\" Arai",
"Tsuki Morimoto",
"Odaya Kana",
"Kawasaki Torikuma",
"Okura Tano",
"Shun Tanaka",
"Ryosei Mamoru",
"Sosuke Matsumura",
"Ayano Kurogane",
"Atoko Kagami",
"PROJEKT: Anima",
"Toto Jikogoto",
"Shiro Hinode",
"Keiji Mizuno",
"Daichi Shige",
"Kyuketsuki Yami",
"Mei Sakai",
"Hiruma Toyonobu",
"Joe McFuck",
"Haru Hikobe",
"Yoi Takajo",
"Aiden Fang",
"Kotarou Nakamoto",
"Isei Fukase",
"Hideo Kojima",
"Yuujiro Kana",
"Eiji Arata",
"Yuujiro Zaou",
"Toi Usotsuki",
"Ryuu Tataki",
"Kazuya Mizukai",
"Koichi Arai",
"Yumi Nekomura",
"Atri Sakurame",
"Ino Kagato",
"Kaibutsu Shinkai",
"Wes Bluemarine",
"Jonas Herrmann",
"Lucius Domitrius Aurelianus",
"Kana Hanazawa",
"Eugeniusz z Agros",
"Sayori Sugi",
"Makomo Kanroji",
"Akira Annaishi",
"Sarasa \"Astra Takahashi",
"Shioko Touoka",
"Ryoko Sasakawa",
"Kuri Sachi",
"Koji Kato",
"Kaho Matsumoto",
"Kiyoshi Kawata",
"Hisoka Ishikawa",
"Iris Tobisawa",
"Bunta \"Anne Emmy\" Aato",
"Yuri Chisuga",
"4",
"Yoko Tawaru",
"Nozomi Akasaka",
"Minato Aamada",
"Keanu Reeves",
"Sadako Anzu Takeuchi",
"Aki Shiba",
"Shingo Taka",
"Sho Tanaka",
"Akiko Ai",
"Yoihon Yorunochou",
"Asuka Nogareru",
"Sara Nakao",
"Nakao Yorikane",
"Okitsugu Yokoi",
"Gustave Bacque",
"Amane \"Dori\" Ito",
"Isamu Harada",
"Eri Taketemo",
"Akemi Kawamura",
"Toshio Rakkutan",
"Luisa Andrea Fernandez Carriedo",
"Kyo Moriya",
"Aiko Oikagane",
"Lu-Kasai Han",
"Haruka Harukawa",
"Haruno Harukawa",
"Amai Sonshitsu",
"Orio Kaneko",
"Asami Nakashima",
"Rin Noonoana",
"Hideki Takano",
"Haruhiko Ueno",
"Kaito (KaShi)",
"Naki Maion",
"Minori Sugihara",
"Taro Miyazaki",
"Ruru Sone",
"Hoshi Nakagawa",
"Casper Inomori",
"Ren Mikami",
"Kazumi Seikoru",
"Hori Gennosuke",
"Yasashi Hiroyuki",
"Kai Matsudaira",
"Yuta Yoshimura",
"Jiro Shiraishi",
"Manudel Del Polo",
"Atsushi Kizuishi",
"Azashi Kizuishi",
"Natsu Akanekki",
"Touru Iwasato",
"Maya Nakajima",
"Yukio Muranabe",
"Kazuya Muranabe",
"Allariel la Pâquerette",
"Lasos Nasselis",
"Miko Saki",
"Cins Chiodi",
"Niel U. Cian",
"Tsuginori Sotomura",
"Taethh Van'lkar",
"Fleur Blanche",
"Parthenope Kuramoto",
"Andaman Kuramoto",
"Kideicraiss",
"Bancu",
"Nox",
"Mika Hegedűs",
"Amator Kun",
"Dominus Diego Deusament",
"Kuriko Yamada",
"Kazuhiro Motoyoshi",
"Ikki von Schönerer",
"Kotaro Kato",
"Yukio Arata",
"Hikaru Takeshi",
"Airi Yazawa",
"Misao",
"Ozu Nakamura",
"Kaku Suika",
"Kinomi Suika",
"Wari Suika",
"Manjirou Sekki",
"Tsukimi Furuya",
"Yubi Kazou",
"Noriko Mori",
"Mitsuko Tobe",
"Leonard Chesterton",
"Yami Watanabe",
"Asami Kusaru",
"Raitae Omoiden",
"Ama Sora",
"Thomas Nazo Kamisato",
"Ikuo \"Adjustment\" Egao",
"Takahiro Ichiro",
"Anzu Yamana",
"Rin Komagata",
"Mai Monogama",
"Sui Ninomiya",
"Katsumi Fokusi",
"Koizumi Fuhijo",
"Faith Megumi Millhaven",
"Shokino Hakaru",
"Kang Il-Cheon",
"Sadao Saiichi",
"Amari Ko",
"Kaito Kichigai",
"Keisuke Yamaguchi",
"Shinzou Arabanaj",
"Hideaki Chino",
"Minami Nakamura",
"Shin Nomura",
"Fuko Haiki",
"Eszter Szarvas",
"Yae Okamoto",
"Shirushi Ayuzawa",
"Katashi Ishihara",
"Mako Tamuro",
"Yorikane / Utano Tomo",
"Enigma",
"Sukuro Kadoku",
"Asbjorn Thorsen",
"Tetsuya Sen",
"Anri Aomine",
"Satoshi Nii",
"Airi Tanaka",
"Kamiko Nishimura",
"Makoto Nishimura",
"Akihiko Minami",
"Mari Kobayashi",
"Neiko Watanabe",
"Mamoru Yamaguchi",
"Jessie Shimura",
"Kotone Inomori",
"Sakura Yamaguchi",
"Satoru Chiba",
"Mizuki Tobe",
"Shinn Sakai",
"Shimada Hoshi",
"Masao Takeda",
"Grace Campbell",
"Ayumi Shiratori",
"Kazuki Nishihara",
"Aoi Suiren",
"Lang Lien Igarashi",
"River Barbeu",
"Yumeko Robaku-Ai",
"Rie Okano",
"Chisai Suraisu",
"Seno Chiyotanda",
"Minoru Sakurai",
"Tsuyo Asukuru",
"Aohato Otosora",
"Taru okama",
"Mimi",
"Miriel Tasartir",
"Julius (SA-003)",
"Arita Kurou Harmony",
"Vulture",
"Chang Ji-Shi",
"Shisuki Kugeru",
"Zabawa Nazwisko",
"Taichleach Ó Maoilsheachlainn [...]",
"Echo",
"Zero",
"Zack / Cody Martin",
"Bakin Kirigaya",
"Kuno Yukika",
"Amai \"Yukio\" Yakusoku",
"Asaka Hatsumi",
"Akuno Nakamura",
"Fumiko Seki",
"Kiryoku Kyūmori",
"Eari Ueyama",
"Jacques Ito",
"Zoe Ito",
"Saori Miyahara",
"Neon",
"Chinatsu Maki",
"Daichi Watanabe",
"Bao Dai",
"Eden Chiyotanda-Berthier",
"Shotaro Kikui",
"Okashi Tachibana",
"Shion Chiyotanda",
"Goh Munemori",
"Max V. Shikansen",
"Suzanne Neville",
"Shinyo Geiku",
"Airi Evergloam",
"Takeshi \"Scarr\" Osada",
"Kennedy Roseling",
"Kurt \"K\" Kortez",
"Price Noa",
"Satoru Nikuma",
"Shizen Nijuu",
"Kiseki Kanemaru",
"Odile Jean de Chelles-Blanche",
"Kodama Dagon",
"Leah Dixon",
"Kabe Uchigawa",
"Takato Saijou",
"Koya Takaki",
"Nini Aradóttir",
"Regulus Larsen",
"Korri Nattfari",
"Mei Sanakawa",
"Ryotaro Nagasawa",
"Koharu Baba",
"Susu Tanlùn",
"Lucia Velasquez",
"Ginjiro Shigeto",
"Joji Amemori",
"Hikari Usuguraku",
"Riyeko \"Rini\" Katachi",
"Vadast",
"Ruisa Hamusuta",
"Uwaki Rakyu",
"Hiroshi Tanaka",
"Sayuri Takato",
"Shizuki Supita",
"Fia Njall",
"NekoKiku",
"Ayame Shimizu",
"Masumi Matsudaira",
"Litsuo Onyo",
"Tetsui Toyama",
"Mirai Ebinuma",
"Aki-chan",
"Adam",
"Namiko Sasaki",
"Tekkeitsertok Hiraeth",
"Dougen Zazen",
"Yoichi Otonami",
"Tsukasa Homuzu",
"Kame Kaneko",
"Hotaka Matsuyama",
"Tsuyoshi Tsukamatta",
"Ritsu Asahara",
"Yuna Hayashida",
"Mirai Jikken",
"Shourai Jikken",
"Kassandra Laflamme",
"Odoru Sakuranomiya",
"Łoszak",
"Carmen Santi-Medina",
"Liya Mitsu",
"David Suneku",
"Liam Sean O'Neil",
"Ivo Vlach-Kovařík",
"Oinas",
"Morris Bever",
"Kaoru Yoshida",
"Maitake Kinoko",
"Ezra Garicia",
"Joan Lafayette",
"Adeline F. V. Chesborough",
"Rosalind Weathers",
"Alexander Osagawara",
"Monosagi Matsumura",
"Tiiu Patshea",
"Yanjian Baihe",
"Hime Akinome",
"Goro Shiraishi",
"Francis Halford",
"Dorai Sanhobu",
"Theodore Hastings",
"Evelyn Fraser-Hastings",
"Samira \"Vex\" Delacroix",
"Mirai Netsuzo",
"Tetsu Utakazu",
"Kaya Sachiko",
"Laura A. Kowalska",
"Veeti Kaede",
"Jerry Palmer",
"Ladah “Niima” Kaiss al-Andrah",
"Cassiopeia Hastings",
"Hachiko Arikanechi",
"Bijuu Sumifude",
"Maud Nibbles",
"Sven von Husch",
"Riku Kiryuuin",
"Quinzel",
"Elvira Perez-Mejía",
"Akari Tenko",
"Aoba Tenko",
"Taika Shardul",
"Neptun",
"Rokuro Bushida"
	};


	bool dealt = false;
	bool passed = false;
	int dif = 2;
	int CDif = 2;
	entt::entity questText;
	entt::entity partyText;
	entt::entity moneyText;

	std::vector<Q> Quests = {
		{"Exterminate the rats in the basement of the local tavern ", 1},
		{"Exterminate the rats in the local storage ", 2},
		{"Exterminate the rats in the sewers ", 3},
		{"Exterminate the mutated rats in the sewers ", 4},
		{"Recover the barman's favourite ladle after he lost it drinking ", 0},
		{"Recover the local witch's hat from the wizard who stole it. ", 2},
		{"Recover the ancient sigil protecting the city, stolen by the cultists ", 3},
		{"Recover the ancient sigil protecting the city, stolen by a hostile nation  ", 4},
		{"Kill the goblins harassing the travellers on the roads ", 1},
		{"Kill the goblins swarming the roads  ", 2},
		{"Kill the goblins swarming the forest  ", 3},
		{"Kill the goblin king  ", 2},
		{"Capture the goblin king ", 3},
		{"Find proof that Count Alucard is a vampire ", 1},
		{"Find proof that Duchess Esmeralda is a vampire  ", 3},
		{"Find proof that Empress Mathilda is a vampire  ", 5},
		{"Exterminate the cult festering in the high city  ", 1},
		{"Exterminate the cult festering in the low city ", 2},
		{"Exterminate the cult who took hold of the slums  ", 3},
		{"Save the mayor, who has been kidnapped by goblins  ", 1},
		{"Save the mayor, who has been kidnapped by a dangerous cult  ", 2},
		{"Save the mayor, who has gotten deadly sick  ", 2},
		{"Save the mayor, who has been kidnapped by a dragon  ", 3},
		{"Save the mayor, who has fallen into a magical sleep ", 3},
		{"Save the mayor, who has been bewitched by the evil goddess ", 4},
		{"Wake up a minor god, who has fallen into a deep slumber ", 2},
		{"Kill a minor god, who is bothering the countryside  ", 3},
		{"Wake up a major god, who has fallen into a deep slumber ", 3},
		{"Kill a major god, who is laying waste to the country ", 5},
		{"Kill a demon who was summoned by a previous party ", 1},
		{"Kill a demon who was summoned by a cult ", 2},
		{"Kill a demon who has coalesced into a physical form through sheer willpower ", 1},
		{"Summon UwU Demon. ", 2},

	};
	std::shared_ptr<Texture> t_n;
	std::shared_ptr<Texture> t_g;

	void call(entt::registry& registry)
	{
		if (dealt)
			return;

		if (passed)
			return;

		dealt = true;
		auto view = registry.view<EnemyTokenComp, Sprite, ScreenPosition>();

		for (auto [entity, token, sprite, pos] : view.each())
		{
			int n = std::rand() % buttons.size();
			token.BetState = n;
			pos.x = registry.get<ScreenPosition>(buttons.at(n)).x + (std::rand() % 40) + 20;
			pos.y = registry.get<ScreenPosition>(buttons.at(n)).y + (std::rand() % 40) + 20;
		}
	}

	void reset(entt::registry& registry)
	{
		dealt = false;
		for (size_t i = 0; i < 8; i++)
		{
			registry.get<Sprite>(buttons.at(i)).texture = t_n;
		}
		auto view = registry.view<EnemyTokenComp, Sprite, ScreenPosition>();

		for (auto [entity, token, sprite, pos] : view.each())
		{
			int n = std::rand() % buttons.size();
			token.BetState = -1;
			pos.x = 1900;
			pos.y = 1900;
		}

		auto view1 = registry.view<TokenComp, Sprite, ScreenPosition>();
		int i = 0;
		for (auto [entity, token, sprite, pos] : view1.each())
		{
			token.BetState = -1;
			token.Holding = false;
			pos = { 480 + (float)(i * 65), 40 };
			i ++ ;
		}
		int r = std::rand() % Quests.size();
		dif = Quests.at(r).dif;

		registry.get<Text>(questText).content = Quests.at(r).s;

		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(Characters.begin(), Characters.end(), g);

		int d = std::rand() % PartyMembers.size();
		CDif = PartyMembers.at(d).dif;
		registry.get<Text>(partyText).content = PartyMembers.at(d).t + "\n\n" + replaceCharacters(PartyMembers.at(d).s, Characters);

		if (money <= 0)
			money = 100;

	}

	result run()
	{
		result r;
		switch (dif)
		{
		case 0:
			r.die = false;
			r.dieOne = chance(30 - CDif * 10);
			r.love = chance(10);
			r.survive = !r.dieOne;
			r.sucess = chance(95);
			r.fail = !r.sucess;
			r.lvlUp = chance(10);
			if (r.dieOne)
				r.trauma = chance(50);
			else
				r.trauma = chance(0);
			
			break;
		case 1:
			r.die = chance(20 - (CDif * 10));
			if (r.die)
			{
				r.dieOne = false;
				r.fail = true;
				r.love = false;
				r.lvlUp = false;
				r.sucess = false;
				r.survive = false;
				r.trauma = false;
			}
			else
			{
				r.dieOne = chance(30 - CDif * 10);
				r.love = chance(10);
				r.survive = !r.dieOne;
				r.sucess = chance(60 + CDif * 20);
				r.fail = !r.sucess;
				r.lvlUp = chance(30);
				if (r.dieOne)
					r.trauma = chance(90);
				else
					r.trauma = chance(0);
			}
			break;

		case 2:
			r.die = chance(40 - (CDif * 20));
			if (r.die)
			{
				r.dieOne = false;
				r.fail = true;
				r.love = false;
				r.lvlUp = false;
				r.sucess = false;
				r.survive = false;
				r.trauma = false;
			}
			else
			{
				r.dieOne = chance(30 - CDif * 10);
				r.love = chance(50);
				r.survive = !r.dieOne;
				r.sucess = chance(40 + CDif * 20);
				r.fail = !r.sucess;
				r.lvlUp = chance(50);
				if (r.dieOne)
					r.trauma = chance(60);
				else
					r.trauma = chance(10);
			}
			break;

		case 3:
			r.die = chance(60 - (CDif * 20));
			if (r.die)
			{
				r.dieOne = false;
				r.fail = true;
				r.love = false;
				r.lvlUp = false;
				r.sucess = false;
				r.survive = false;
				r.trauma = false;
			}
			else
			{
				r.dieOne = chance(50 - CDif * 10);
				r.love = chance(30);
				r.survive = !r.dieOne;
				r.sucess = chance(50 + CDif * 10);
				r.fail = !r.sucess;
				r.lvlUp = chance(50);
				if (r.dieOne)
					r.trauma = chance(50);
				else
					r.trauma = chance(20);
			}
			break;

		case 4:
			r.die = chance(100 - (CDif * 10));
			if (r.die)
			{
				r.dieOne = false;
				r.fail = true;
				r.love = false;
				r.lvlUp = false;
				r.sucess = false;
				r.survive = false;
				r.trauma = false;
			}
			else
			{
				r.dieOne = chance(70 - CDif * 10);
				r.love = chance(20);
				r.survive = !r.dieOne;
				r.sucess = chance(20 + CDif * 10);

				r.fail = !r.sucess;
				r.lvlUp = chance(70);
				
				r.trauma = r.dieOne;
			}
			break;

		case 5:
			r.die = chance(100 - (CDif * 5));
			if (r.die)
			{
				r.dieOne = false;
				r.fail = true;
				r.love = false;
				r.lvlUp = false;
				r.sucess = false;
				r.survive = false;
				r.trauma = false;
			}
			else
			{
				r.dieOne = true;
				r.fail = false;
				r.love = chance(20);
				r.lvlUp = true;
				r.sucess = true;
				r.survive = false;
				r.trauma = true;
			}
			break;

		default:
			r.die = true;
			r.dieOne = false;
			r.fail = true;
			r.love = false;
			r.lvlUp = false;
			r.sucess = false;
			r.survive = false;
			r.trauma = false;
			break;
		}

		return r;
	}

	int money = 100;

	void points(entt::registry& registry, result& r)
	{
		int part = money / 10;
		auto view = registry.view<TokenComp, Sprite, ScreenPosition>();

		for (auto [entity, token, sprite, pos] : view.each())
		{
			if (token.BetState > -1)
			{
				if (registry.get<Sprite>(buttons[token.BetState]).texture == t_g)
				{
					bool a = false;
					auto view1 = registry.view<EnemyTokenComp, Sprite, ScreenPosition>();

					for (auto [entity, tt, aaa, bbb] : view1.each())
					{
						if (tt.BetState == token.BetState)
						{
							a = true;
							break;
						}
					}

					if (a)
						money += part * 1;
					else
						money += part * 3;
				}
				else
				{
					money -= part;
				}
			}
		}
	
	
		registry.get<Text>(moneyText).content = std::to_string(money);
	}
	
	void pass(entt::registry& registry)
	{
		if (!dealt)
			return;

		passed = true;

		result r = run();

		if (r.survive)
		{
			registry.get<Sprite>(buttons.at(0)).texture = t_g;
		}
		if (r.die)
		{
			registry.get<Sprite>(buttons.at(1)).texture = t_g;
		}
		if (r.fail)
		{
			registry.get<Sprite>(buttons.at(2)).texture = t_g;
		}
		if (r.dieOne)
		{
			registry.get<Sprite>(buttons.at(3)).texture = t_g;
		}
		if (r.lvlUp)
		{
			registry.get<Sprite>(buttons.at(4)).texture = t_g;
		}
		if (r.trauma)
		{
			registry.get<Sprite>(buttons.at(5)).texture = t_g;
		}
		if (r.love)
		{
			registry.get<Sprite>(buttons.at(6)).texture = t_g;
		}
		if (r.sucess)
		{
			registry.get<Sprite>(buttons.at(7)).texture = t_g;
		}
		points(registry, r);
		delay = 3;
	}

	void Update(entt::registry& registry)
	{
		if (delay > 0)
		{
			delay -= dt;
			return;
		}
		else
		{
			if (passed)
			{
				passed = false;
				reset(registry);
			}
			
		}

		buttonUpdate(registry);
		settlePos(registry);
	}

};

*/