#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>


#include <SDL3/SDL.h>
#include <SDL3_Image/SDL_image.h>
#include <spdlog/spdlog.h>
#include <KitsuEngine/KitsuneEngine.hpp>
#include "Components.hpp"

#include "Systems/RendererSystem.hpp"
#include "Systems/AnimatorSystem.hpp"
#include "Systems/RemoveAfterDelaySystem.hpp"
#include "Systems/UISystem.hpp"
#include "Systems/DebugMoveSystem.hpp"
#include "Systems/TipSystem.hpp"
#include "Systems/CakeMakerSystem.hpp"
#include "Systems/DialogueSystem.hpp"
#include "Systems/GameManager.hpp"

#include <cassert>
#include <iostream>
#include <regex>
#include <algorithm>
#include <random>


/*
TODO:
Arts

*/

class App : public Scene
{
private:
    
    std::shared_ptr<Font> font = std::make_shared<Font>("assets/Font/munro.ttf");
    
    DialogueLine* CreateDialogueList(const std::vector<DialogueLine*>& disconnected)
    {
        if (disconnected.size() == 0)
            return nullptr;

        for (size_t i = 0; i < disconnected.size() - 1; i++)
        {
            disconnected.at(i)->next = disconnected.at(i + 1);
        }
        return disconnected.at(0);
    }



    void SetupHall(const std::shared_ptr<Texture> t_right)
    {
        std::shared_ptr<Texture> t_counter = CreateTexture("assets/GPX/Counter.png");
        std::shared_ptr<Texture> t_wall = CreateTexture("assets/GPX/Wall.png");
        std::shared_ptr<Texture> t_bg = CreateTexture("assets/GPX/bg.png");

        // BG, layer 2
        {
            entt::entity bg = m_registry.create();
            m_registry.emplace<Position>(bg, Position{0,0});
            Sprite s_bg;

            s_bg.layerOrder = 2;
            s_bg.texture = t_bg;
            s_bg.sizeX = 1920;
            s_bg.sizeY = 1080;
            m_registry.emplace<Sprite>(bg, s_bg);
        }

        // wall, layer 6
        {
            entt::entity wall = m_registry.create();
            m_registry.emplace<Position>(wall, Position{ 0,1080 - 600 });
            Sprite s_wall;

            s_wall.layerOrder = 6;
            s_wall.texture = t_wall;
            s_wall.sizeX = 400;
            s_wall.sizeY = 600;
            m_registry.emplace<Sprite>(wall, s_wall);
        }

        // counter, layer 6
        {
            entt::entity counter = m_registry.create();
            m_registry.emplace<Position>(counter, Position{ 400, 1080 - 400 });
            Sprite s_counter;

            s_counter.layerOrder = 6;
            s_counter.texture = t_counter;
            s_counter.sizeX = 1000;
            s_counter.sizeY = 400;
            m_registry.emplace<Sprite>(counter, s_counter);
        }

       

        // right, layer 7
        {
            entt::entity right = m_registry.create();
            m_registry.emplace<Position>(right, Position{ 1800, 1080 - 280 - 30 });
            
            Sprite s_right;
            s_right.layerOrder = 7;
            s_right.texture = t_right;
            s_right.sizeX = 40;
            s_right.sizeY = 280;  
            s_right.useTextureRect = true;
            s_right.textureRect = { 0, 0, 40, 280 };
            m_registry.emplace<Sprite>(right, s_right);

            TextureButton tb;
            tb.ClickRect = { 0,0,40,280 };
            tb.HoverRect = { 40,0,40,280 };
            m_registry.emplace<TextureButton>(right, tb);

            MoveCameraEffector mce;
            mce.dx = 1920;
            mce.dy = 0;
            m_registry.emplace<MoveCameraEffector>(right, mce);


        }

    }


    std::pair<DialogueLine*, DialogueLine*> scene4Main()
    {
        DialogueLine* dl0 = new DialogueLine("Miss Manager", "Good evening!");
        DialogueLine* dl1 = new DialogueLine("You", "Good evening");
        DialogueLine* dl2 = new DialogueLine("Miss Manager", "How was your first day?");

        DialogueLine* dl3 = new DialogueLine("You", "Good.");
        DialogueLine* dl4 = new DialogueLine("Miss Manager", "Very good");
        DialogueLine* dl5 = new DialogueLine("You", "Mhm");

        DialogueLine* dl6 = new DialogueLine("You", "The police came.");
        DialogueLine* dl7 = new DialogueLine("Miss Manager", "Oh.");
        DialogueLine* dl8 = new DialogueLine("Miss Manager", "You didn't rat us out, right?");

        DialogueLine* dl9 = new DialogueLine("You", "I totally did.");
        DialogueLine* dl10 = new DialogueLine("Miss Manager", "Not good you know, give me half of the price though.");
        DialogueLine* dl11 = new DialogueLine("You", "What!? Why!?");
        DialogueLine* dl12 = new DialogueLine("Miss Manager", "It's in the contract...");
        DialogueLine* dl13 = new DialogueLine("You", "Shit...");

        DialogueLine* dl14 = new DialogueLine("You", "Of course not!");
        DialogueLine* dl15 = new DialogueLine("Miss Manager", "That's my boy, I knew you'd be a good addition to the team");


        DialogueLine* dl16 = new DialogueLine("You", "So I will go pack myself and see you tomorrow.");
        DialogueLine* dl17 = new DialogueLine("Miss Manager", "See you tomorrow...");
        DialogueLine* dl18 = new DialogueLine("You", "*MC left*");
        DialogueLine* dl19 = new DialogueLine("Miss Manager", "...");
        DialogueLine* dl20 = new DialogueLine("Miss Manager", "...");
        DialogueLine* dl21 = new DialogueLine("Miss Manager", "...");
        DialogueLine* dl22 = new DialogueLine("Miss Manager", "That's the end of the game, you know?");
        DialogueLine* dl23 = new DialogueLine("Miss Manager", "Yes I am talking to you");
        DialogueLine* dl24 = new DialogueLine("Miss Manager", "Thank you very much for playing, I hope you enjoyed it");

        DialogueLine* dl25 = new DialogueLine("???", "Yes, it was fun");
        DialogueLine* dl26 = new DialogueLine("Miss Manager", "Great to hear, bye bye~");

        DialogueLine* dl27 = new DialogueLine("???", "I hated it.");
        DialogueLine* dl28 = new DialogueLine("Miss Manager", "lmao");


        CreateDialogueList({ dl6, dl7, dl8 });
        CreateDialogueList({ dl9, dl10, dl11, dl12, dl13, dl16 });
        CreateDialogueList({ dl14, dl15, dl16 });
        CreateDialogueList({ dl0, dl1, dl2 });
        CreateDialogueList({ dl25, dl26 });
        CreateDialogueList({ dl27, dl28 });
        CreateDialogueList({ dl3, dl4, dl5, dl16, dl17, dl18, dl19, dl20, dl21, dl22, dl23, dl24 });

        dl2->options.push_back(dl3);
        dl2->options.push_back(dl6);
        
        dl8->options.push_back(dl9);
        dl8->options.push_back(dl14);

        dl24->options.push_back(dl25);
        dl24->options.push_back(dl27);



        return std::pair<DialogueLine*, DialogueLine*>(dl0, dl6);
    }

 
    std::pair<DialogueLine*, DialogueLine*> scene3Main(DialogueLine** e2)
    {
        DialogueLine* dl0 = new DialogueLine("Beatrice", "Good afternoon.");
        DialogueLine* dl1 = new DialogueLine("You", "Good morning miss, what can I get you?");
        DialogueLine* dl2 = new DialogueLine("Beatrice", "I've got an information that you can sell some ammo.");
        DialogueLine* dl3 = new DialogueLine("Manager in your head", "(*Don't trust the coooooops-*)");

        DialogueLine* dl4 = new DialogueLine("You", "Yes, that's true");
        DialogueLine* dl5 = new DialogueLine("Beatrice", "Oh.");
        DialogueLine* dl6 = new DialogueLine("Beatrice", "That went really easily, can you give me some, I need a proof.");
        DialogueLine* dl7 = new DialogueLine("Beatrice", "And I can't check behind the counter...");
        DialogueLine* dl8 = new DialogueLine("You", "Ye, coming up!");

        DialogueLine* dl9 = new DialogueLine("You", "This is a cake store miss.");
        DialogueLine* dl10 = new DialogueLine("Beatrice", "... I... I know.");
        DialogueLine* dl11 = new DialogueLine("You", "So, can I get you something?");
        DialogueLine* dl12 = new DialogueLine("Beatrice", "... something sweet please.");
        DialogueLine* dl13 = new DialogueLine("Beatrice", "I like Ammo as topings!");
        DialogueLine* dl14 = new DialogueLine("You", "...");

        *e2 = dl8;


        CreateDialogueList({ dl0, dl1, dl2, dl3 });
        CreateDialogueList({ dl4, dl5, dl6, dl7, dl8 });
        CreateDialogueList({ dl9, dl10, dl11, dl12, dl13, dl14 });
        
        dl3->options.push_back(dl4);
        dl3->options.push_back(dl9);
        dl14->next = dl12;
        dl8->next = dl6;


        return std::pair<DialogueLine*, DialogueLine*>(dl0, dl14);
    }

    DialogueLine* scene3Good()
    {
        DialogueLine* dl0 = new DialogueLine("Beatrice", "That looks informative!");
        DialogueLine* dl1 = new DialogueLine("You", "I suppose so?");
        DialogueLine* dl2 = new DialogueLine("Bianka", "Thank you!");
        DialogueLine* dl3 = new DialogueLine("Bianka", "I will get that raise finally! Bye!");
        DialogueLine* dl4 = new DialogueLine("You", "Goodbye");



        CreateDialogueList({ dl0, dl1, dl2, dl3, dl4 });
        return dl0;
    }

    DialogueLine* scene3Bad()
    {
        DialogueLine* dl0 = new DialogueLine("Beatrice", "You hope it is tasty.");
        DialogueLine* dl1 = new DialogueLine("You", "I can assure you.");
        DialogueLine* dl2 = new DialogueLine("Beatrice", "... I will come back.");
        DialogueLine* dl3 = new DialogueLine("You", "Can't wait!");
        DialogueLine* dl4 = new DialogueLine("You", "Enjoy");
        DialogueLine* dl5 = new DialogueLine("Beatrice", "Bye.");
        DialogueLine* dl6 = new DialogueLine("You", "Goodbye");



        CreateDialogueList({ dl0, dl1, dl2, dl3, dl4, dl5, dl6 });
        return dl0;
    }



    std::pair<DialogueLine*, DialogueLine*> scene2Main()
    {
        DialogueLine* dl0 = new DialogueLine("Carl", "Hello.");
        DialogueLine* dl1 = new DialogueLine("You", "Good morning mister, what can I get you?");
        DialogueLine* dl2 = new DialogueLine("Carl", "...");
        DialogueLine* dl3 = new DialogueLine("Carl", "I would like some \"green\" chocolate cake, can you do that for me?");


        DialogueLine* dl4 = new DialogueLine("You", "\"Green?\"");
        DialogueLine* dl5 = new DialogueLine("Carl", "You know, inside it. The item...");
        DialogueLine* dl6 = new DialogueLine("You", "Yeee... I will try to do it.");
        DialogueLine* dl7 = new DialogueLine("Carl", "Thanks");

        DialogueLine* dl8 = new DialogueLine("You", "Okay~ Gotta go");
        DialogueLine* dl9 = new DialogueLine("Carl", "I will wait here");

        DialogueLine* dl10 = new DialogueLine("Carl", "Could you get me some chocolate cake with some \"Greens\"?");
        DialogueLine* dl11 = new DialogueLine("You", "Coming straight away");
   


        CreateDialogueList({ dl0, dl1, dl2, dl3 });
        CreateDialogueList({ dl4, dl5, dl6, dl7, dl8, dl9, dl11 });
        dl11->next = dl10;
        dl10->next = dl11;

        dl3->options.push_back(dl4);
        dl3->options.push_back(dl8);

        return std::pair<DialogueLine*, DialogueLine*>(dl0, dl11);
    }

    DialogueLine* scene2Good()
    {
        DialogueLine* dl0 = new DialogueLine("Carl", "Smells just right!");
        DialogueLine* dl1 = new DialogueLine("You", "I bet it does, enjoy");
        DialogueLine* dl2 = new DialogueLine("Carl", "Thank you");
        DialogueLine* dl3 = new DialogueLine("Carl", "Bye");
        DialogueLine* dl4 = new DialogueLine("You", "Goodbye");



        CreateDialogueList({ dl0, dl1, dl2, dl3, dl4 });
        return dl0;
    }

    DialogueLine* scene2Bad()
    {
        DialogueLine* dl0 = new DialogueLine("Carl", "Are you sure that's it?");
        DialogueLine* dl1 = new DialogueLine("You", "Positive");
        DialogueLine* dl2 = new DialogueLine("Carl", "... I don't know man, I hope you are right");
        DialogueLine* dl3 = new DialogueLine("You", "Thank you for the trust");
        DialogueLine* dl4 = new DialogueLine("Carl", "...");



        CreateDialogueList({ dl0, dl1, dl2, dl3, dl4 });
        return dl0;
    }



    std::pair<DialogueLine*,DialogueLine*> scene1Main()
    {
        DialogueLine* dl0 = new DialogueLine("Bianka", "Hii!!!");
        DialogueLine* dl1 = new DialogueLine("You", "Good morning miss, what can I get you?");
        DialogueLine* dl2 = new DialogueLine("Bianka", "Hmmm~");
        DialogueLine* dl3 = new DialogueLine("Bianka", "What about some lemon cake!");
        DialogueLine* dl4 = new DialogueLine("Bianka", "With chocolate topping please!");
        DialogueLine* dl5 = new DialogueLine("You", "Anything else?");
        DialogueLine* dl6 = new DialogueLine("Bianka", "No!");

        DialogueLine* dl7 = new DialogueLine("Bianka", "Some lemon cake please!");


        CreateDialogueList({ dl0, dl1, dl2, dl3, dl4, dl5, dl6});
        dl6->next = dl7;
        dl7->next = dl4;

        return std::pair<DialogueLine*, DialogueLine*>(dl0,dl6);
    }

    DialogueLine* scene1Good()
    {
        DialogueLine* dl0 = new DialogueLine("Bianka", "That looks tasty!");
        DialogueLine* dl1 = new DialogueLine("You", "It sure is");
        DialogueLine* dl2 = new DialogueLine("Bianka", "Thank you!");
        DialogueLine* dl3 = new DialogueLine("Bianka", "Byee!");
        DialogueLine* dl4 = new DialogueLine("You", "Goodbye");



        CreateDialogueList({ dl0, dl1, dl2, dl3, dl4 });
        return dl0;
    }
    
    DialogueLine* scene1Bad()
    {
        DialogueLine* dl0 = new DialogueLine("Bianka", "That's not what I wanted!");
        DialogueLine* dl1 = new DialogueLine("You", "Really?");
        DialogueLine* dl2 = new DialogueLine("Bianka", "Yes!");
        DialogueLine* dl3 = new DialogueLine("Bianka", "I am not buying it!");
        DialogueLine* dl4 = new DialogueLine("Bianka", "Bye!!");
        DialogueLine* dl5 = new DialogueLine("You", "... shit.");



        CreateDialogueList({ dl0, dl1, dl2, dl3, dl4, dl5 });
        return dl0;
    }

    
    DialogueLine* scene0()
	{
		DialogueLine* dl0 = new DialogueLine("Miss Manager", "Hey there, I've seen you came for your first day!");
		DialogueLine* dl01 = new DialogueLine("You", "Yes!");
		DialogueLine* dl1 = new DialogueLine("Miss Manager", "Are you ready for your first day?");
		dl0->next = dl01;
		dl01->next = dl1;



		DialogueLine* dl20 = new DialogueLine("You", "Yes?");
		DialogueLine* dl21 = new DialogueLine("Miss Manager", "Awesome, you will get everything when you see it, good luck!");
		DialogueLine* dl22 = new DialogueLine("You", "Wait-");
		DialogueLine* dl2 = CreateDialogueList({ dl20, dl21, dl22 });

		DialogueLine* dl30 = new DialogueLine("You", "No?");
		DialogueLine* dl31 = new DialogueLine("Miss Manager", "Quite a shame! I've already signed a contract with you, so you have no choice");
		DialogueLine* dl32 = new DialogueLine("You", "I guess-");
		DialogueLine* dl33 = new DialogueLine("Miss Manager", "Well see you then!");
		DialogueLine* dl3 = CreateDialogueList({ dl30, dl31, dl32, dl33 });

		dl1->options.push_back(dl2);
		dl1->options.push_back(dl3);

        return dl0;
	}
    
    void SetupClients()
    {
        std::shared_ptr<Texture> t_manager = CreateTexture("assets/GPX/Miss manager.png");
        std::shared_ptr<Texture> t_char2 = CreateTexture("assets/GPX/char2.png");
        std::shared_ptr<Texture> t_char3 = CreateTexture("assets/GPX/char3.png");
        std::shared_ptr<Texture> t_char4 = CreateTexture("assets/GPX/char4.png");

        // manager, layer 4
        {
            Client* manager = new Client();
            manager->character = entt::null;
            manager->start = scene0(); 
            manager->correct = nullptr; // This one does not have it
            manager->endLoop = nullptr;
            manager->incorrect = nullptr;
            manager->item = -1;
            manager->special = -1;
            manager->type = -1;

            manager->pos = Position{ 450, 1080 - (3508 / 4) - 100 };

            Sprite s_manager;
            s_manager.layerOrder = 4;
            s_manager.texture = t_manager;
            s_manager.sizeX = 2480 / 4; // 2480
            s_manager.sizeY = 3508 / 4;  // 3508

            manager->spr = s_manager;
            
            getSystemIfExists<GameManager>()->clients.push(manager);
        }

        // char2, layer 4
        {
            Client* char2 = new Client();
            char2->character = entt::null;
            std::pair<DialogueLine*, DialogueLine*> pp = scene1Main();
            char2->start = pp.first;
            char2->correct = scene1Good();
            char2->endLoop = pp.second;  
            char2->incorrect = scene1Bad();

            char2->item = 4; // lemon
            char2->special = -1;
            char2->type = 1; // 1

            char2->pos = Position{ 450, 1080 - (3508 / 4) - 100 };

            Sprite s_bianka;
            s_bianka.layerOrder = 4;
            s_bianka.texture = t_char2;
            s_bianka.sizeX = 2480 / 4; // 2480
            s_bianka.sizeY = 3508 / 4;  // 3508

            char2->spr = s_bianka;

            getSystemIfExists<GameManager>()->clients.push(char2);
        }

        // char3, layer 4
        {
            Client* char2 = new Client();
            char2->character = entt::null;
            std::pair<DialogueLine*, DialogueLine*> pp = scene2Main();
            char2->start = pp.first;
            char2->correct = scene2Good();
            char2->endLoop = pp.second;
            char2->incorrect = scene2Bad();

            char2->item = 1; // choco
            char2->special = 2; // weed
            char2->type = -1;

            char2->pos = Position{ 450, 1080 - (3508 / 4) - 100 };

            Sprite s_bianka;
            s_bianka.layerOrder = 4;
            s_bianka.texture = t_char3;
            s_bianka.sizeX = 2480 / 4; // 2480
            s_bianka.sizeY = 3508 / 4;  // 3508

            char2->spr = s_bianka;

            getSystemIfExists<GameManager>()->clients.push(char2);
        }

        // char4, layer 4
        {
            DialogueLine* e2;
            Client* char2 = new Client();
            char2->character = entt::null;
            std::pair<DialogueLine*, DialogueLine*> pp = scene3Main(&e2);
            char2->start = pp.first;
            char2->correct = scene3Good();
            char2->endLoop = pp.second;
            char2->endLoop2 = e2;
            char2->incorrect = scene3Bad();

            char2->item = -1;
            char2->special = 3;
            char2->type = -1;

            char2->pos = Position{ 450, 1080 - (3508 / 4) - 100 };

            Sprite s_bianka;
            s_bianka.layerOrder = 4;
            s_bianka.texture = t_char4;
            s_bianka.sizeX = 2480 / 4; // 2480
            s_bianka.sizeY = 3508 / 4;  // 3508

            char2->spr = s_bianka;

            getSystemIfExists<GameManager>()->clients.push(char2);
        }

        // manager, layer 4
        {
            Client* manager = new Client();
            manager->character = entt::null;
            manager->start = scene4Main().first;
            manager->correct = nullptr; // This one does not have it
            manager->endLoop = nullptr;
            manager->incorrect = nullptr;
            manager->item = -1;
            manager->special = -1;
            manager->type = -1;

            manager->pos = Position{ 450, 1080 - (3508 / 4) - 100 };

            Sprite s_manager;
            s_manager.layerOrder = 4;
            s_manager.texture = t_manager;
            s_manager.sizeX = 2480 / 4; // 2480
            s_manager.sizeY = 3508 / 4;  // 3508

            manager->spr = s_manager;

            getSystemIfExists<GameManager>()->clients.push(manager);
        }

    }

    // Add tips to buttons, to check how check pedals
    void SetupMilikingRoom(const std::shared_ptr<Texture> t_right, UISystem* uis)
    {
        std::shared_ptr<Texture> t_bg = CreateTexture("assets/GPX/bag.png");
        std::shared_ptr<Texture> t_machines = CreateTexture("assets/GPX/machines.png");
        std::shared_ptr<Texture> t_buttons = CreateTexture("assets/GPX/buttons.png");
        std::shared_ptr<Texture> t_pedals = CreateTexture("assets/GPX/pedals.png");
        std::shared_ptr<Texture> t_cake = CreateTexture("assets/GPX/cakes.png");
        std::shared_ptr<Texture> t_icing = CreateTexture("assets/GPX/icings.png");
        std::shared_ptr<Texture> t_choco = CreateTexture("assets/GPX/choco.png");
        std::shared_ptr<Texture> t_items = CreateTexture("assets/GPX/items.png");

        uis->SetCakeMakerSystem(addSystem<CakeMakerSystem>(t_cake, t_icing, t_choco, t_items));
        addSystem<GameManager>(getSystemIfExists<DialogueSystem>(), getSystemIfExists<CakeMakerSystem>());

        SetupClients();

        // "right" - left, layer 7
        {
            entt::entity right = m_registry.create();
            m_registry.emplace<Position>(right, Position{ 1920 + 60, 1080 - 280 - 30 });

            Sprite s_right;
            s_right.layerOrder = 7;
            s_right.texture = t_right;
            s_right.sizeX = 40;
            s_right.sizeY = 280;
            s_right.useTextureRect = true;
            s_right.textureRect = { 0, 0, 40, 280 };
            s_right.flipX = true;
            m_registry.emplace<Sprite>(right, s_right);

            TextureButton tb;
            tb.ClickRect = { 0,0,40,280 };
            tb.HoverRect = { 40,0,40,280 };
            m_registry.emplace<TextureButton>(right, tb);

            MoveCameraEffector mce;
            mce.dx = -1920;
            mce.dy = 0;
            m_registry.emplace<MoveCameraEffector>(right, mce);


        }

        // right, layer 7
        {
            entt::entity right = m_registry.create();
            m_registry.emplace<Position>(right, Position{ 1920 + 1800, 1080 - 280 - 30 });

            Sprite s_right;
            s_right.layerOrder = 7;
            s_right.texture = t_right;
            s_right.sizeX = 40;
            s_right.sizeY = 280;
            s_right.useTextureRect = true;
            s_right.textureRect = { 0, 0, 40, 280 };
            m_registry.emplace<Sprite>(right, s_right);

            TextureButton tb;
            tb.ClickRect = { 0,0,40,280 };
            tb.HoverRect = { 40,0,40,280 };
            m_registry.emplace<TextureButton>(right, tb);

            MoveCameraEffector mce;
            mce.dx = 1920;
            mce.dy = 0;
            m_registry.emplace<MoveCameraEffector>(right, mce);


        }
    
        // BG, layer 2
        {
            entt::entity bg = m_registry.create();
            m_registry.emplace<Position>(bg, Position{ 1920 + 0,0 });
            Sprite s_bg;

            s_bg.layerOrder = 2;
            s_bg.texture = t_bg;
            s_bg.sizeX = 1920;
            s_bg.sizeY = 1080;
            m_registry.emplace<Sprite>(bg, s_bg);
        }
    
        // Machines, layer 4
        {
            std::vector<Position> positions = { 
            Position{ 2400,0 },
            Position{ 2570,0 },
            Position{ 2740,0 },
            Position{ 2909,0 },
            Position{ 3056,0 },
            Position{ 3226,0 } };

            std::vector<SDL_Scancode> keys = {
                SDL_SCANCODE_Z,
                SDL_SCANCODE_X,
                SDL_SCANCODE_C,
                SDL_SCANCODE_V,
                SDL_SCANCODE_B,
                SDL_SCANCODE_N,
            };

            for (size_t i = 0; i < positions.size(); i++)
            {
                entt::entity machine = m_registry.create();
                m_registry.emplace<Position>(machine, Position{positions.at(i).x, positions.at(i).y});

                Sprite s_machine;
                s_machine.layerOrder = 4;
                s_machine.texture = t_machines;
                s_machine.sizeX = 230;
                s_machine.sizeY = 590;
                s_machine.useTextureRect = true;
                s_machine.textureRect = { (float)i * 230, 0, 230, 590 };
                m_registry.emplace<Sprite>(machine, s_machine);

                /*
                DebugMove dm;
                dm.key = keys.at(i);
                dm.speed = 50;
                m_registry.emplace<DebugMove>(machine, dm);*/
            }
        
        }

        // Buttons, layer 4
        {
            std::vector<Position> positions = {
            Position{ 2088, 794 },
            Position{ 2088, 888 },
            Position{ 2088, 973 },
            Position{ 2250, 888 }, 
            Position{ 2405, 794 },
            Position{ 2405, 888 },
            Position{ 2405, 973 },
            Position{ 2574, 794 },
            Position{ 2574, 888 },
            Position{ 2574, 973 },
            Position{ 2730, 794 },
            Position{ 2730, 888 },
            Position{ 2730, 973 },
            Position{ 2890, 794 },
            Position{ 2890, 888 },
            Position{ 2890, 973 },
            Position{ 3056, 794 },
            Position{ 3056, 888 },
            Position{ 3056, 973 },
            Position{ 3206, 794 },
            Position{ 3206, 888 },
            Position{ 3206, 973 },
            Position{ 3340, 794 },
            Position{ 3460, 794 },
            Position{ 3460, 884 } };

            std::vector<SDL_Scancode> keys = {
                SDL_SCANCODE_Z,
                SDL_SCANCODE_X,
                SDL_SCANCODE_C,
                SDL_SCANCODE_V,
                SDL_SCANCODE_B,
                SDL_SCANCODE_N,
            };

            for (size_t i = 0; i < positions.size(); i++)
            {
                entt::entity button = m_registry.create();
                m_registry.emplace<Position>(button, Position{ positions.at(i).x, positions.at(i).y });

                Sprite s_button;
                s_button.layerOrder = 4;
                s_button.texture = t_buttons;
                s_button.sizeX = 90;
                s_button.sizeY = 110;
                s_button.useTextureRect = true;
                s_button.textureRect = {0, (float)i*90, 110, 90 };
                m_registry.emplace<Sprite>(button, s_button);

                TextureButton tb;
                tb.ClickRect = { 0, (float)i * 90, 110, 90 };
                tb.HoverRect = { 110, (float)i * 90, 110, 90 };
                m_registry.emplace<TextureButton>(button, tb);

                int pos = 1;
                if (i < 3)
                    pos = 1;
                else if (i == 3)
                    pos = 2;
                else if (i <= 21)
                    pos = (int)((i + 5) / 3);
                else if (i == 22)
                    pos = 9;
                else
                    pos = 10;

                CakeButtonEffector cbe;
                cbe.id = i + 5;
                cbe.position = pos;
                m_registry.emplace<CakeButtonEffector>(button, cbe);


                /*
                if (i< keys.size())
                {
                    DebugMove dm;
                    dm.key = keys.at(i);
                    dm.speed = 100;
                    m_registry.emplace<DebugMove>(button, dm);
                }*/

            }

        }
    
        // Pedals, layer 5
        {
            std::vector<Position> positions = {
            Position{ 1930,0 },
            Position{ 2082,0 },
            Position{ 2230,0 },
            Position{ 2376,0 } };

            std::vector<std::string> tips =
            {
                "Add chocolate cake layer",
                "Add strawberry cake layer",
                "Add standard cake layer",
                "Add lemon cake layer"
            };

            std::vector<SDL_Scancode> keys = {
                SDL_SCANCODE_Z,
                SDL_SCANCODE_X,
                SDL_SCANCODE_C,
                SDL_SCANCODE_V
            };

            for (size_t i = 0; i < positions.size(); i++)
            {
                entt::entity pedal = m_registry.create();
                m_registry.emplace<Position>(pedal, Position{ positions.at(i).x, positions.at(i).y });

                Sprite s_pedal;
                s_pedal.layerOrder = 5;
                s_pedal.texture = t_pedals;
                s_pedal.sizeX = 140;
                s_pedal.sizeY = 200;
                s_pedal.useTextureRect = true;
                s_pedal.textureRect = { 0, (float)i * 200, 140, 200 };
                m_registry.emplace<Sprite>(pedal, s_pedal);

                TextureButton tb;
                tb.ClickRect = { 0, (float)i * 200, 140, 200 };
                tb.HoverRect = { 140, (float)i * 200, 140, 200 };
                m_registry.emplace<TextureButton>(pedal, tb);

                Tip t;
                t.tip = tips.at(i);
                m_registry.emplace<Tip>(pedal, t);

                CakeButtonEffector cbe;
                cbe.id = i + 1;
                cbe.position = 1;
                m_registry.emplace<CakeButtonEffector>(pedal, cbe);


                /*
                DebugMove dm;
                dm.key = keys.at(i);
                dm.speed = 50;
                m_registry.emplace<DebugMove>(pedal, dm);*/
            }

        }

    }

    void SetupRestingRoom(const std::shared_ptr<Texture> t_right)
    {
        // "right" - left, layer 7
        {
            entt::entity right = m_registry.create();
            m_registry.emplace<Position>(right, Position{ 1920 + 1920 + 60, 1080 - 280 - 30 });

            Sprite s_right;
            s_right.layerOrder = 7;
            s_right.texture = t_right;
            s_right.sizeX = 40;
            s_right.sizeY = 280;
            s_right.useTextureRect = true;
            s_right.textureRect = { 0, 0, 40, 280 };
            s_right.flipX = true;
            m_registry.emplace<Sprite>(right, s_right);

            TextureButton tb;
            tb.ClickRect = { 0,0,40,280 };
            tb.HoverRect = { 40,0,40,280 };
            m_registry.emplace<TextureButton>(right, tb);

            MoveCameraEffector mce;
            mce.dx = -1920;
            mce.dy = 0;
            m_registry.emplace<MoveCameraEffector>(right, mce);


        }

        //text
        {
            entt::entity op = m_registry.create();
            m_registry.emplace<Position>(op, Position{ 1920 + 1920 + 600,400 });

            Text t;
            t.font = font;
            t.color = { 0, 0, 0, 255 };
            t.content = "Day 1";
            t.fontSize = 64;
            t.xSize = 550;
            t.ySize = 200;
            m_registry.emplace<Text>(op, t);
        }

    }

public:
    App(Game* game) : Scene(game)
    {
    }

    virtual void Init()
    {
        RendererSystem* rs = addSystem<RendererSystem>();
        rs->camXPos = 1920 / 2;
        rs->camYPos = 1080 / 2;
        addSystem<AnimatorSystem>();
        UISystem* uis = addSystem<UISystem>();
        addSystem<DebugMoveSystem>();
        addSystem<TipSystem>(font);
        addSystem<DialogueSystem>(font, CreateTexture("assets/GPX/DialogueButton.png"));
       

        spdlog::info("Scene got init");


        std::shared_ptr<Texture> t_right = CreateTexture("assets/GPX/right.png");
    
    
        SetupHall(t_right);
        SetupMilikingRoom(t_right, uis);
        SetupRestingRoom(t_right);

        
    }
       
};

std::pair<int,int> GetScreenSize()
{
    SDL_DisplayID primaryDisplay = SDL_GetPrimaryDisplay();
    if (primaryDisplay == 0)
    {
        spdlog::error("Failed to get primary display: {}", SDL_GetError());
    }
    else
    {
        SDL_Rect bounds;
        if (!SDL_GetDisplayBounds(primaryDisplay, &bounds))
        {
            spdlog::error("SDL_GetDisplayBounds failed: {}", SDL_GetError());
        }
        else
        {
            int screenWidth = bounds.w;
            int screenHeight = bounds.h;
            spdlog::info("Primary screen size: {} x {}", screenWidth, screenHeight);
            return std::pair<int, int>(screenWidth, screenHeight);
        }
    }

    return { 0,0 };
}

Game* game;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
    srand(time(NULL));

    WindowParams wp;
    wp.Fullscreen = true;

    int res = KitsuEngineInit(1920, 1080, "Not just a front Cake Game", &wp);
    if (res == -1)
    {
        return SDL_APP_FAILURE;
    }
    game = new Game();
    App* mainScene = new App(game);
    game->StartGame(mainScene);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    game->MainLoop();

    if (game->shouldStop())
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;

}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    bool quit = game->HandleEvents(event);
    if (quit)
        return SDL_APP_SUCCESS;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    delete game;
    KitsuEngineClean();
    SDL_Quit();
}


