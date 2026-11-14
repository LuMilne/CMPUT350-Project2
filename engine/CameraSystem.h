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
        auto& manager = engine->GetEntityManager();
        auto& window = engine->GetWindowManager();

        //Checks if we have a camera id yet or not. Saves checking everytime as we only have one
        //  thing thats being followed by the camera
        if (mCameraID == NULL) {
            for (auto& entities : manager) {
                if (manager.template HasComponent<CameraFollower>(entities.GetID())) {
                    mCameraID = entities.GetID();
                    break;
                }
            }
            if (mCameraID == NULL) {
                return kContinue;
            }
        }
        auto& camera = manager.template GetComponent<CameraComponent>(mCameraID);
        auto& player = manager.template GetComponentStorage<CameraFollower>().mStorage[0].entityID;

        //TODO: Add shake, Following, Clamping
        // camera.position.x += camera.speed;

        // Camera Follow
        // Get player position on screen
        const auto& playerLoc = manager.template GetComponent<LocationComponent>(player);
        const Point2D playerScreenLoc = window.WorldToWindow(playerLoc.loc);
        const Rect bounds = window.GetWindowBounds();
        float playerNormLoc = playerScreenLoc.x / bounds.width;
        // Check hard boundaries (outer 10% margins)
        if(playerNormLoc <= 0.1f || playerNormLoc >= 0.9f) {
            // Hard-track player
            float normVec = getVectorToBoundary(playerNormLoc, 0.1f, 0.9f);
            // Convert vector to world-coordinates
            float worldVec = window.WindowToWorld(normVec * bounds.width);
            // Hard-translate camera
            camera.position.x += worldVec;
        }
        // Check soft boundaries (outer 30% margins)
        else if(playerNormLoc <= 0.3f || playerNormLoc >= 0.7f) {
            // Smooth-track player
            float normVec = getVectorToBoundary(playerNormLoc, 0.3f, 0.7f);
            // Convert vector to world-coordinates
            float worldVec = window.WindowToWorld(normVec * bounds.width);
            // Smooth-translate camera
            camera.position = lerp_x(
                camera.position,
                {camera.position.x + worldVec, camera.position.y},
                mv_alpha
            );
        }
        auto& playerMovement = manager.template GetComponent<MovementComponent>(player);
        // If player is not moving, track within 40% margins
        if(playerMovement.velocity.x == 0.0f) {
            // Smooth-center player
            float normVec = getVectorToBoundary(playerNormLoc, 0.4f, 0.6f);
            // Convert vector to world-coordinates
            float worldVec = window.WindowToWorld(normVec * bounds.width);
            // Smooth-translate camera
            camera.position = lerp_x(
                camera.position,
                {camera.position.x + worldVec, camera.position.y},
                st_alpha
            );
        }

        Point2D center = camera.position;

        window.SetWorldCenter(center);

        return kContinue;
    }

private:
    EntityID mCameraID{};
    float mv_alpha = 0.3f;
    float st_alpha = 0.1f;

    /**
     * @brief Returns smallest vector from min.max boundaries to location (along x-axis)
     * 
     * @param location the target point on the x-axis
     * @param min the minimum(leftmost) boundary
     * @param max the maximum(rightmost) boundary
     * 
     * @returns the difference from the closest boundary to the location
     */
    float getVectorToBoundary(float location, float min, float max) {
        float diffToMin = location - min;
        float diffToMax = location - max;
        if(std::abs(diffToMax) < std::abs(diffToMin)) return diffToMax;
        else return diffToMin;
    }

    Point2D lerp_x(Point2D a, Point2D b, float alpha) {
        return {
            (1-alpha) * a.x + alpha * b.x,
            a.y
        };
    }
};

}  // namespace ECSEngine
