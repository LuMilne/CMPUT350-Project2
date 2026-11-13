#pragma once

#include "Keyboard.h"

namespace ECSEngine {

const float kMaxRunVelocity = 15;
const float kDashVelocity = 50;
const float kJumpVelocity = 25;
const float kGroundRunAcceleration = 5;
const float kAirRunAcceleration = 4;
const float kDashInterval = 25;

template <typename... Components>
class InputSystem : public System<Components...> {
public:
    const std::string& GetName() const override {
        static std::string name("InputSystem");
        return name;
    }

    tSystemReturn Run(EngineView<Components...>* engine) override { 
        // Get window
        auto& winMg = engine->GetWindowManager();
        auto& window = winMg.GetWindow();
        // Get input component
        auto& entMg = engine->GetEntityManager();
        auto& inputStorage = entMg.GetComponentStorage<InputComponent>();
        InputComponent& playerInput = inputStorage.mStorage[0]; // Player should be first and only entity with InputComponent
        // Get player entity
        auto player = entMg.GetEntity(inputStorage.mapToEntity[0]);
        // Store and clear key events from previous frame
        playerInput.lastKeydown = playerInput.keydown;
        playerInput.keydown.reset();
        // Poll for key events in window
        while(std::optional<sf::Event> event = window->pollEvent()) {
            // Populate InputComponent bitsets
            if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {    // Keypress events
                playerInput.keydown[keyPressed->code] = 1;
            }
        }
        // Process key events (keys pressed or new key released)
        if(playerInput.keydown.any() || (~playerInput.keydown & playerInput.lastKeydown).any()) {
            // Left/right keypress
            if(playerInput.keyDown[Key::L] && playerInput.keyDown[Key::R]) {    // Simultaneous left/right commands cancel out
                playerStop(player);
            }
            else if(playerInput.keyDown[Key::L]) {  // Move left
                playerMove(player, false);
            }
            else if(playerInput.keyDown[Key::R]) {  // Move right
                playerMove(player, true);
            }
            else if(playerInput.lastKeydown[Key::L] && playerInput.lastKeydown[Key::R]) {   // Movement keyRelease event
                playerStop(player);
            }
            // Jump keypress
            if(playerInput.keydown[Key::Z] & ~playerInput.lastKeydown[Key::Z]) {   // New keypress
                playerJump(player);
            }
            else if(playerInput.keydown[Key::Z] & playerInput.lastKeydown[Key::Z]) {    // Keyhold

            }
            else {
                playerFall(player);
            }
        }
        return kContinue; 
    }
private:
    void playerMove(EntityManager::iterator player, bool right) {   // TODO: Implement acceleeration in horizontal movement (slower accel when airborne)
        auto& movementComp = entMg.GetComponent<MovementComponent>(player->GetID());
        if(right) {
            movementComp.velocity.x = 3;
        }
        else {
            movementComp.velocity.x = -3;
        }
    }
    void playerStop(EntityManager::iterator player) {
        auto& movementComp = entMg.GetComponent<MovementComponent>(player->GetID());
        movementComp.velocity = 0;
    }
    void playerJump(EntityManager::iterator player) {   // TODO: implement variable jump height
    }
    void playerFall(EntityManager::iterator player) {

    }
    float groundAccel = 4;
    float airAccel = 3;
};

}  // namespace ECSEngine
