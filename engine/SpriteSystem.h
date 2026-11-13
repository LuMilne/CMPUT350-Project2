#pragma once

#include <bitset>
#include <unordered_map>
#include <stdexcept>

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
        auto& entMg = engine->GetEntityManager();
        auto& winMg = engine->GetWindowManager();
        auto& spriteMg = engine->GetSpriteManager();
        auto window = winMg.GetWindow();
        // Iterate and draw any entity with a SpriteComponent
        for(auto it = entMg.cbegin(); it != entMg.cend(); it++) {
            // Check for SpriteComponent and LocationComponent
            const EntityID id = it->GetID();
            if(entMg.template HasComponent<SpriteComponent>(id)) {
                // Extract sprite and info
                const SpriteComponent spriteComp = entMg.template GetComponent<SpriteComponent>(id);
                sf::Sprite sprite = spriteMg.GetSprite(spriteComp.textureID);    // Just a copy. Don't edit sprite info
                // Determine if this is a world entity or a screen entity
                if(spriteComp.worldSpace) { // World entity
                    if(!entMg.template HasComponent<LocationComponent>(id)) {
                        throw std::runtime_error("Worldspace entity has no LocationComponent.");
                    }

                    // Retrieve world location
                    const LocationComponent location = entMg.template GetComponent<LocationComponent>(id);

                    // Convert to screen space
                    // Position (world position is location component + sprite component offset)
                    Point2D screenPosition = winMg.WorldToWindow(Point2D(
                        location.loc.x + spriteComp.bounds.topLeft.x,
                        location.loc.y + spriteComp.bounds.topLeft.y
                    ));

                    sprite.setPosition({screenPosition.x, screenPosition.y});

                    // Scale (from texture dimzensions to screen-space dimensions)
                    const auto spriteSize = sprite.getTexture().getSize();
                    const float screenWidth = winMg.WorldToWindow(spriteComp.bounds.width);
                    const float screenHeight = winMg.WorldToWindow(spriteComp.bounds.height);

                    sprite.setScale({
                        screenWidth / float(spriteSize.x),
                        screenHeight / float(spriteSize.y)
                    });
                } else {
                    const LocationComponent location = entMg.template GetComponent<LocationComponent>(id);
                    sprite.setPosition({location.loc.x + spriteComp.bounds.topLeft.x, location.loc.y + spriteComp.bounds.topLeft.y});
                    auto textureSize = sprite.getTextureRect().size;
                    sprite.setScale({spriteComp.bounds.width / textureSize.x, spriteComp.bounds.height / textureSize.y});
                }

                // Draw sprite
                window->draw(sprite);
            }
        }
        return kContinue; 
    }
};

}  // namespace ECSEngine
