#pragma once

#include <bitset>
#include <unordered_map>

#include "CameraComponent.h"
#include "EntityManager.h"
#include "MathUtil.h"
#include "SpriteManager.h"
#include "SystemManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

template <typename... Components>
class SpriteSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("SpriteSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override { 
        const auto entMg = engine->GetEntityManager();
        // Iterate and draw any entity with a SpriteComponent
        for(const_iterator it = entMg->cbegin(), it != entMg->cend(), it++) {
            if(entMg->HasComponent<SpriteComponent>(it*)) {
                const auto sprite = entMg->GetComponent<SpriteComponent>(it*);
                const auto location = entMg->GetComponent<LocationComponent>(it*);
                
            }
        }
        return kContinue; 
    }
};

}  // namespace ECSEngine
