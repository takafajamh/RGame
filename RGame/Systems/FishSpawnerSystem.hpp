#pragma once
#include "../Components.hpp"
#include <random>


static std::mt19937 rng{ std::random_device{}() };


class FishSpawnerSystem
{
private:
    float m_spawnTimer = 0;
    float m_spawnDelay = 0.1f;
    float m_radius = 1600.f;
    int m_maxNearby = 36;

public:

    void DespawnFarFishes(entt::registry& registry, const Position& playerPos)
    {
        const float maxDistance = m_radius * 2.5f;

        auto view = registry.view<Position, FishComponent>();
        for (auto entity : view)
        {
            auto& pos = view.get<Position>(entity);
            float dx = pos.x - playerPos.x;
            float dy = pos.y - playerPos.y;

            if ((dx * dx + dy * dy) > (maxDistance * maxDistance)) {
                registry.destroy(entity);
            }
            
        }

    }

    int CountFishesNear(entt::registry& registry, const Position& playerPos)
    {
        int s = 0;
        auto view = registry.view<Position, FishComponent>();
        for (auto entity : view)
        {
            auto& pos = view.get<Position>(entity);
            float dx = pos.x - playerPos.x;
            float dy = pos.y - playerPos.y;

            if ((dx * dx + dy * dy) < (m_radius * m_radius))
            {
                s++;
            }

        }

        return s;
    }

    void SpawnFishNear(entt::registry& registry, const Position& playerPos,std::shared_ptr<Texture> t_Sum1)
    {
        constexpr float minDistance = 500.0f;  // Minimum distance from player
        constexpr float maxDistance = 1200.0f;  // Maximum distance from player

        // Generate random angle and distance within the ring
        std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14);
        std::uniform_real_distribution<float> radiusDist(minDistance, maxDistance);

        float angle = angleDist(rng);
        float distance = radiusDist(rng);

        float offsetX = std::cos(angle) * distance;
        float offsetY = std::sin(angle) * distance;

        float spawnX = playerPos.x + offsetX;
        float spawnY = playerPos.y + offsetY;


        if (spawnX < 400)
            return;


        if (spawnY < 600)
            return;

        std::uniform_real_distribution<float> dist(-2.0f, 2.0f);
        float speed = dist(rng);


        entt::entity Sum = registry.create();
        registry.emplace<Sprite>(Sum,
            Sprite{
                (float)t_Sum1->getSDL()->w / 4 * 3,
                (float)t_Sum1->getSDL()->h * 3,
                4,
                true,
                {0,0,48,24}, // optional, default anyway
                t_Sum1,
                (speed > 0)
            });

        registry.emplace<Position>(Sum, Position{spawnX, spawnY}); // pozycja



        Animation sumAnim;
        sumAnim.name = "idle";

        for (int i = 0; i < 4; ++i)
        {
            Frame frame;
            frame.duration = 0.15f;
            frame.frame = SDL_FRect{ static_cast<float>(i * 48), 0.0f, 48.0f, 24.0f };                

            sumAnim.frames.push_back(frame);
        }

        Animator animator;
        animator.anims.push_back(sumAnim);
        animator.looping = true;
        animator.ToPlay = "idle";


        registry.emplace<Animator>(Sum, animator);


        

        registry.emplace<FishComponent>(Sum, FishComponent{ speed, 0}); //TOOOOOOOOOOOOOOO CHANGEEEEEEEEEEEE

    }

    void Update(const Position& playerPos, entt::registry& registry, std::shared_ptr<Texture> t_Sum1)
    {
        const int baseNearby = 36;
        const float scaleFactor = 0.005f;

        float distFromOrigin = std::sqrt(playerPos.x * playerPos.x + playerPos.y * playerPos.y);
        m_maxNearby = static_cast<int>(baseNearby + distFromOrigin * scaleFactor);


        m_spawnTimer += dt;

        DespawnFarFishes(registry, playerPos);

        if (m_spawnTimer < m_spawnDelay) return;

        m_spawnTimer = 0;

        int nearbyCount = CountFishesNear(registry, playerPos);
        if (nearbyCount >= m_maxNearby) return;

        SpawnFishNear(registry, playerPos, t_Sum1);
    }

};