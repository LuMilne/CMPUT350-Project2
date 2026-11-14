#pragma once

#include <random>
#include <vector>

namespace ECSEngine {

template <typename... Components>
class SpawnSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("SpawnSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {

        auto& manager = engine->GetEntityManager();
        std::vector<EntityID> readySpawners;

        // Search entities ready to spawn
        for (const auto& entities : manager) {
            const EntityID entityID = entities.GetID();

            // Skip non-spawning entities
            if (!manager.template HasComponent<SpawnComponent>(entityID)) {
                continue;
            }

            // Skip non-star and empty spawners (TODO: Implement player spawns)
            auto& spawnComponent = manager.template GetComponent<SpawnComponent>(entityID);
            if (spawnComponent.spawnType != "stars" || spawnComponent.totalSpawns <= 0) {
                continue;
            }

            // Decrement unfinished spawn timers
            if (spawnComponent.timeToNextSpawn > 0) {
                --spawnComponent.timeToNextSpawn;
                continue;
            }

            // Set spawner as ready to spawn
            spawnComponent.timeToNextSpawn = spawnComponent.timeBetweenSpawns;
            readySpawners.push_back(entityID);
        }

        // Randomize spawn velocity
        std::uniform_real_distribution<float> horizontalVelocity(-5.f, 5.f);
        std::uniform_real_distribution<float> verticalVelocity(-20.f, -5.f);
        for (const EntityID spawnerID : readySpawners) {
            const auto& spawner = manager.template GetComponent<SpawnComponent>(spawnerID);
            const Point2D position = manager.template GetComponent<LocationComponent>(spawnerID).loc;
            const SpriteID starSprite = spawner.spawnTextureID;
            const Point2D velocity{
                horizontalVelocity(mRandomEngine),
                verticalVelocity(mRandomEngine)
            };

            auto star = manager.CreateEntity("star");
            manager.template AddComponent<LocationComponent>(star, LocationComponent{position});
            manager.template AddComponent<GravityComponent>(star, GravityComponent{{0.f, 2.f}});
            manager.template AddComponent<MovementComponent>(star, MovementComponent{velocity});
            manager.template AddComponent<SpriteComponent>(star, SpriteComponent(starSprite, Rect({0, 0}, 64, 64), true));

            CollisionComponent collisionComp{};
            collisionComp.staticEntity = 0;
            collisionComp.collisionBox = Rect({0, 0}, 64, 64);
            collisionComp.previousCollisionBox = collisionComp.collisionBox;
            manager.template AddComponent<CollisionComponent>(star, collisionComp);
        }

        return kContinue;
    }

private:
    std::mt19937 mRandomEngine{std::random_device{}()}; // Handles random velocity of spawns
};

}  // namespace ECSEngine
