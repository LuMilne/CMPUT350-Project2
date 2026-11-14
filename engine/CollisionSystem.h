#pragma once
#include "InputComponent.h"
#include <algorithm>
namespace ECSEngine {

template <typename... Components>
class CollisionSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("CollisionSystem");
        return name;
    }

    bool Collides (Rect player, Rect tile) {
        if (
            player.topLeft.x < tile.topLeft.x + tile.width &&
            player.topLeft.x + player.width> tile.topLeft.x &&
            player.topLeft.y < tile.topLeft.y + tile.height &&
            player.topLeft.y + player.height > tile.topLeft.y
        ) {
            return true;
        }
        return false;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override {

        auto& manager = engine->GetEntityManager();
        auto& window = engine->GetWindowManager();
        std::vector<EntityID> mTiles;

        for (auto& entities : manager) {
            if (manager.template HasComponent<InputComponent>(entities.GetID())) {
                mPlayerId = entities.GetID();
            }
        }

        //Makes a list of our tiles
        for (auto& entities : manager) {
            const EntityID eID = entities.GetID();
            if (manager.template HasComponent<CollisionComponent>(eID)) {
                if (manager.template GetComponent<CollisionComponent>(eID).staticEntity == 1) {
                    mTiles.push_back(eID);
                }
            }
        }


        //Get Player Components
        auto& playerCol = manager.template GetComponent<CollisionComponent>(mPlayerId);
        auto& playerMovement = manager.template GetComponent<MovementComponent>(mPlayerId);
        auto& playerLoc = manager.template GetComponent<LocationComponent>(mPlayerId);

        playerCol.onGround = playerCol.onCeiling = playerCol.onLeftWall = playerCol.onRightWall = 0;

        //Start Looping through the tiles
        for (EntityID e : mTiles) {
            const auto& tileCol = manager.template GetComponent<CollisionComponent>(e);
            const auto& tileLoc = manager.template GetComponent<LocationComponent>(e);


            //Convert to window coords to ensure both are in the same space. Then calc bounding boxes with location accounted for
            const Rect playerWindow = window.WorldToWindow(Rect(
                {playerLoc.loc.x + playerCol.collisionBox.topLeft.x, playerLoc.loc.y + playerCol.collisionBox.topLeft.y},
                playerCol.collisionBox.width, playerCol.collisionBox.height));

            const Rect tileWindow = window.WorldToWindow(Rect(
                {tileLoc.loc.x + tileCol.collisionBox.topLeft.x, tileLoc.loc.y + tileCol.collisionBox.topLeft.y},
                tileCol.collisionBox.width, tileCol.collisionBox.height));


            if (!Collides(playerWindow, tileWindow)) {
                continue;
            }

            //Check to see most likely collision direction
            float bottom = playerWindow.topLeft.y + playerWindow.height - tileWindow.topLeft.y;
            float top = tileWindow.topLeft.y + tileWindow.height - playerWindow.topLeft.y;
            float right = playerWindow.topLeft.x + playerWindow.width - tileWindow.topLeft.x;
            float left = tileWindow.topLeft.x + tileWindow.width - playerWindow.topLeft.x;

            //Prevents Clipping while also properly maintaining collision
            const Rect oldBox = window.WorldToWindow(playerCol.previousCollisionBox);

            //Bottom Col
            if (oldBox.topLeft.y + oldBox.height <= tileWindow.topLeft.y) {
                screenShake(engine, manager, playerMovement.velocity);
                playerLoc.loc.y -= window.WindowToWorld(bottom);
                playerMovement.velocity.y = 0;
                playerCol.onGround = 1;
            //Right Col
            } else if (oldBox.topLeft.x + oldBox.width <= tileWindow.topLeft.x) {
                screenShake(engine, manager, playerMovement.velocity);
                playerLoc.loc.x -= window.WindowToWorld(right);
                playerMovement.velocity.x = 0;
                playerCol.onRightWall = 1;
            //Left Col
            } else if (oldBox.topLeft.x >= tileWindow.topLeft.x + tileWindow.width) {
                screenShake(engine, manager, playerMovement.velocity);
                playerLoc.loc.x += window.WindowToWorld(left);
                playerMovement.velocity.x = 0;
                playerCol.onLeftWall = 1;
            //Top Col
            } else if (oldBox.topLeft.y >= tileWindow.topLeft.y + tileWindow.height) {
                screenShake(engine, manager, playerMovement.velocity);
                playerLoc.loc.y += window.WindowToWorld(top);
                playerMovement.velocity.y = 0;
                playerCol.onCeiling = 1;
            }

        }

        return kContinue;
    }

private:
    float shakeThreshold = 20.0f;
    float impactScale = 0.5f;
    /**
     * @brief Applies a ScreenShake component to the camera if player impacts an obstacle at speed
     * 
     * @param manager the entity manager
     * @param velocity the player entity's current velocity
     */
    void screenShake(EngineView<Components...>* engine, EntityManager<Components...>& manager, Point2D velocity) {
        velocity.y = std::max(velocity.y, 0.0f);
        // Get scale of combined velocity
        float totalVel = std::sqrt(velocity.x*velocity.x + velocity.y*velocity.y);
        if(totalVel > shakeThreshold) {
            auto& cameraStorage = manager.template GetComponentStorage<CameraComponent>();
            auto& cameraID = cameraStorage.mapToEntity[0];
            // Don't double-up on CameraShake objects
            if(!manager.template HasComponent<CameraShake>(cameraID)) {
                CameraShake cameraShake = {
                    6,
                    {
                    velocity.x * impactScale,
                    velocity.y * impactScale
                    }
                };
                manager.template AddComponent<CameraShake>(cameraID, cameraShake);
            }
            auto& soundMg = engine->GetSoundManager();
            auto& impactSound = soundMg.GetSound("footstep_grass_003");
            impactSound.play();
        }
    }
    EntityID mPlayerId;

};

}  // namespace ECSEngine
