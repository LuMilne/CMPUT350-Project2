#pragma once

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

        for (const auto& entities : manager) {
            const EntityID entityID = entities.GetID();

            if (!manager.template HasComponent<SpawnComponent>(entityID)) {
                continue;
            }

            auto& spawnComponent = manager.template GetComponent<SpawnComponent>(entityID);
            if (spawnComponent.totalSpawns <=0 || spawnComponent.timeToNextSpawn > 0) {
                continue;
            }

            if (spawnComponent.spawnType == "stars") {
                spawnerID = entityID;
            } else if (spawnComponent.spawnType == "player") {
                playerID = entityID;
            }
        }
        auto& spawner = manager.template GetComponent<SpawnComponent>(spawnerID);
        if (spawner.spawnType == "stars") {
            if (!(spawner.totalSpawns <=0 || spawner.timeToNextSpawn > 0)) {
                spawner.timeToNextSpawn = spawner.timeBetweenSpawns;
                --spawner.totalSpawns;
            }

            const Point2D position = manager.template GetComponent<LocationComponent>(spawnerID);
            const SpriteID starSprite = spawner.spawnTextureID;

            //TODO: INSERT RANDOM DIRECTION (x and y)
            const float movX = -5, movY = 10;

            auto star = manager.CreateEntity("star");
            manager.template AddComponent<LocationComponent>(position);
            manager.template AddComponent<GravityComponent>({0, 2});
            manager.template AddComponent<MovementComponent>({movX, movY});
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
    EntityID spawnerID;
    EntityID playerID;
};

}  // namespace ECSEngine
