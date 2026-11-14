#pragma once

namespace ECSEngine {

template <typename... Components>
class CollisionLateSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("CollisionLateSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto& manager = engine->GetEntityManager();

        for (auto& entity : manager) {
            const EntityID entityID = entity.GetID();

            if (manager.template HasComponent<CollisionComponent>(entityID)) {
                auto& collision = manager.template GetComponent<CollisionComponent>(entityID);
                if (collision.staticEntity) {
                    continue;
                }

                if (!manager.template HasComponent<LocationComponent>(entityID)) {
                    continue;
                }
                auto& location = manager.template GetComponent<LocationComponent>(entityID);
                collision.previousCollisionBox = Rect({collision.collisionBox.topLeft.x + location.loc.x,
                    collision.collisionBox.topLeft.y + location.loc.y}, collision.collisionBox.width, collision.collisionBox.height);
            }
        }

        return kContinue;
    }
};

}  // namespace ECSEngine
