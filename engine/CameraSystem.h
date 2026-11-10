#pragma once

#include <unordered_map>

#include "CameraComponent.h"
#include "Entity.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "SpriteComponent.h"
#include "SystemManager.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

template <typename... Components>
class CameraSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name = "CameraSystem";
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {
        auto manager = engine->GetEntityManager();
        auto& window = engine->GetWindowManager();

        //Checks if we have a camera id yet or not. Saves checking everytime as we only have one
        //  thing thats being followed by the camera
        if (mCameraID == NULL) {
            for (auto& entities : manager) {
                if (entities.template HasComponent<CameraComponent>(entities.GetID())) {
                    mCameraID = entities.GetID();
                    break;
                }
            }
            if (mCameraID == NULL) {
                return kContinue;
            }
        }
        auto& camera = manager.template GetComponent<CameraComponent>(mCameraID);

        //TODO: Add shake, Following, Clamping

        Point2D center = camera.position;

        window.SetWorldCenter(center);

        return kContinue;
    }

private:
    //TODO: GIVE PLAYER A CAMERA COMPONENT SO IT FOLLOWS
EntityID mCameraID{};
};

}  // namespace ECSEngine
