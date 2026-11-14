#pragma once

namespace ECSEngine {

template <typename... Components>
class MovementSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("MovementSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {

        auto& manager = engine->GetEntityManager();
        // auto& window = engine->GetWindowManager();

        for (auto& entities : manager) {
            if (manager.template HasComponent<MovementComponent>(entities.GetID()) && manager.template HasComponent<LocationComponent>(entities.GetID())) {
                mEntityID = entities.GetID();
                break;
            }
        }

        auto& movement = manager.template GetComponent<MovementComponent>(mEntityID);
        auto& location = manager.template GetComponent<LocationComponent>(mEntityID);
        // auto& gravity = manager.template GetComponent<GravityComponent>(mEntityID);

        location.loc.x += movement.velocity.x;
        location.loc.y += movement.velocity.y;


        return kContinue;
    }

private:
    EntityID mEntityID;
};

}  // namespace ECSEngine
