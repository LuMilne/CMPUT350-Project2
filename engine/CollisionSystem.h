#pragma once
#include "InputComponent.h"
#include "ScoreComponent.h"
#include <algorithm>
#include "EntityManager.h"
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
        std::vector<EntityID> mStars;
        std::vector<EntityID> removed;

        //Makes a list of our tiles
        for (auto& entities : manager) {
            const EntityID eID = entities.GetID();
            // Get Player entity
            if (manager.template HasComponent<InputComponent>(eID)) {
                mPlayerId = eID;
            }
            // Get tile entities
            else if (manager.template HasComponent<CollisionComponent>(eID) 
            && !manager.template HasComponent<MovementComponent>(eID)) {
                if (manager.template GetComponent<CollisionComponent>(eID).staticEntity == 1) {
                    mTiles.push_back(eID);
                }
            }
            // Get star entities
            else if (manager.template HasComponent<MovementComponent>(eID) 
            && manager.template HasComponent<CollisionComponent>(eID)
            && eID != mPlayerId) {
                mStars.push_back(eID);
            }
        }


        //Get Player Components
        auto& playerCol = manager.template GetComponent<CollisionComponent>(mPlayerId);
        auto& playerMovement = manager.template GetComponent<MovementComponent>(mPlayerId);
        auto& playerLoc = manager.template GetComponent<LocationComponent>(mPlayerId);

        playerCol.onGround = playerCol.onCeiling = playerCol.onLeftWall = playerCol.onRightWall = 0;

        // Player-Tile collisions
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

            // Check to see most likely collision direction
            float bottom = playerWindow.topLeft.y + playerWindow.height - tileWindow.topLeft.y;
            float top = tileWindow.topLeft.y + tileWindow.height - playerWindow.topLeft.y;
            float right = playerWindow.topLeft.x + playerWindow.width - tileWindow.topLeft.x;
            float left = tileWindow.topLeft.x + tileWindow.width - playerWindow.topLeft.x;

            // Prevents Clipping while also properly maintaining collision
            const Rect oldBox = window.WorldToWindow(playerCol.previousCollisionBox);

            // Bottom Col
            if (oldBox.topLeft.y + oldBox.height <= tileWindow.topLeft.y) {
                screenShake(engine, manager, playerMovement.velocity);
                playerLoc.loc.y -= window.WindowToWorld(bottom);
                playerMovement.velocity.y = 0;
                playerCol.onGround = 1;
            // Right Col
            } else if (oldBox.topLeft.x + oldBox.width <= tileWindow.topLeft.x) {
                screenShake(engine, manager, playerMovement.velocity);
                playerLoc.loc.x -= window.WindowToWorld(right);
                playerMovement.velocity.x = 0;
                playerCol.onRightWall = 1;
            // Left Col
            } else if (oldBox.topLeft.x >= tileWindow.topLeft.x + tileWindow.width) {
                screenShake(engine, manager, playerMovement.velocity);
                playerLoc.loc.x += window.WindowToWorld(left);
                playerMovement.velocity.x = 0;
                playerCol.onLeftWall = 1;
            // Top Col
            } else if (oldBox.topLeft.y >= tileWindow.topLeft.y + tileWindow.height) {
                screenShake(engine, manager, playerMovement.velocity);
                playerLoc.loc.y += window.WindowToWorld(top);
                playerMovement.velocity.y = 0;
                playerCol.onCeiling = 1;
            }
        }
        // Star collisions
        for(EntityID e : mStars) {
            const auto& starCol = manager.template GetComponent<CollisionComponent>(e);
            auto& starLoc = manager.template GetComponent<LocationComponent>(e);
            auto& starMove = manager.template GetComponent<MovementComponent>(e);

            // Star bounding boxes
            const Rect starWindow = window.WorldToWindow(Rect(
                {starLoc.loc.x + starCol.collisionBox.topLeft.x, starLoc.loc.y + starCol.collisionBox.topLeft.y},
                starCol.collisionBox.width, starCol.collisionBox.height));
            
            // Star-Player collision
            const Rect playerWindow = window.WorldToWindow(Rect(
                {playerLoc.loc.x + playerCol.collisionBox.topLeft.x, playerLoc.loc.y + playerCol.collisionBox.topLeft.y},
                playerCol.collisionBox.width, playerCol.collisionBox.height));

            if(Collides(starWindow, playerWindow)) {
                // Play collect sound
                auto& soundMg = engine->GetSoundManager();
                auto& collectSound = soundMg.GetSound("sfx_gem");
                collectSound.play();
                // Increment score
                auto& scoreComp = manager.template GetComponentStorage<ScoreComponent>();
                auto& score = scoreComp.mStorage[0]; // Only one ScoreComponent is stored
                score.score++;
                // Destroy star
                removed.push_back(e);
                manager.RemoveEntity(e);
                continue;
            }

            // Star-Tile collision
            for(EntityID t: mTiles) {
                const auto& tileCol = manager.template GetComponent<CollisionComponent>(t);
                const auto& tileLoc = manager.template GetComponent<LocationComponent>(t);
                const Rect tileWindow = window.WorldToWindow(Rect(
                    {tileLoc.loc.x + tileCol.collisionBox.topLeft.x, tileLoc.loc.y + tileCol.collisionBox.topLeft.y},
                    tileCol.collisionBox.width, tileCol.collisionBox.height));

                if(!Collides(starWindow, tileWindow)) {
                    continue;
                }

                //Check to see most likely collision direction
                float bottom = starWindow.topLeft.y + starWindow.height - tileWindow.topLeft.y;
                float top = tileWindow.topLeft.y + tileWindow.height - starWindow.topLeft.y;
                float right = starWindow.topLeft.x + starWindow.width - tileWindow.topLeft.x;
                float left = tileWindow.topLeft.x + tileWindow.width - starWindow.topLeft.x;

                //Prevents Clipping while also properly maintaining collision
                const Rect oldBox = window.WorldToWindow(starCol.previousCollisionBox);

                //Bottom Col
                if (oldBox.topLeft.y + oldBox.height <= tileWindow.topLeft.y) {
                    starLoc.loc.y -= window.WindowToWorld(bottom);
                    starMove.velocity.y *= -1;
                    break;  // One tile collision per frame
                //Right Col
                } else if (oldBox.topLeft.x + oldBox.width <= tileWindow.topLeft.x) {
                    starLoc.loc.y -= window.WindowToWorld(right);
                    starMove.velocity.x *= -1;
                    break;  // One tile collision per frame
                //Left Col
                } else if (oldBox.topLeft.x >= tileWindow.topLeft.x + tileWindow.width) {
                    starLoc.loc.y -= window.WindowToWorld(left);
                    starMove.velocity.x *= -1;
                    break;  // One tile collision per frame
                //Top Col
                } else if (oldBox.topLeft.y >= tileWindow.topLeft.y + tileWindow.height) {
                    starLoc.loc.y -= window.WindowToWorld(top);
                    starMove.velocity.y *= -1;
                    break;  // One tile collision per frame
                }
            }

            // Star-Star Collision
            for(EntityID s: mStars) {
                if(e == s) continue;    // No self-collision
                if(std::find(removed.begin(), removed.end(), s) != removed.end()) continue;  // Ignore deleted stars
                const auto& otherCol = manager.template GetComponent<CollisionComponent>(s);
                const auto& otherLoc = manager.template GetComponent<LocationComponent>(s);
                const Rect otherWindow = window.WorldToWindow(Rect(
                    {otherLoc.loc.x + otherCol.collisionBox.topLeft.x, otherLoc.loc.y + otherCol.collisionBox.topLeft.y},
                    otherCol.collisionBox.width, otherCol.collisionBox.height));

                if(!Collides(starWindow, otherWindow)) {
                    continue;
                }

                //Prevents Clipping while also properly maintaining collision
                const Rect oldBox = window.WorldToWindow(starCol.previousCollisionBox);

                //Bottom Col
                if (oldBox.topLeft.y + oldBox.height <= otherWindow.topLeft.y) {
                    starMove.velocity.y *= -1;
                //Right Col
                } else if (oldBox.topLeft.x + oldBox.width <= otherWindow.topLeft.x) {
                    starMove.velocity.x *= -1;
                //Left Col
                } else if (oldBox.topLeft.x >= otherWindow.topLeft.x + otherWindow.width) {
                    starMove.velocity.x *= -1;
                //Top Col
                } else if (oldBox.topLeft.y >= otherWindow.topLeft.y + otherWindow.height) {
                    starMove.velocity.y *= -1;
                }
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
    EntityID mPlayerId = size_t(-1);

};

}  // namespace ECSEngine
