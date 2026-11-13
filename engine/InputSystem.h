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
        auto window = winMg.GetWindow();
        // Get input component
        auto& entMg = engine->GetEntityManager();
        auto& inputStorage = entMg.GetComponentStorage<InputComponent>();
        InputComponent& playerInput = inputStorage.mStorage[0]; // Player should be first and only entity with InputComponent
        // Get player entity
        auto player = entMg.GetEntity(inputStorage.mapToEntity[0]);
        // Store and clear key events from previous frame
        playerInput.lastKeydown = playerInput.keydown;
        // Poll for key events in window
        while(std::optional<sf::Event> event = window->pollEvent()) {
            // Moved window close event handling from ECSEngine.h to here
            if (event->is<sf::Event::Closed>()) {
                window->close();
            }
            // Populate InputComponent bitsets
            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {    // Keypress events
                // std::cout << "key pressed!\n";
                playerInput.keydown[static_cast<std::size_t>(keyPressed->scancode)] = 1;
            }
            else if(const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {    // Keyrelease events
                playerInput.keydown[static_cast<std::size_t>(keyReleased->scancode)] = 0;
            }
        }
        // Process key events (keys pressed or new key released)
        if(playerInput.keydown.any() || (~playerInput.keydown & playerInput.lastKeydown).any()) {
            auto& moveComp = entMg.GetComponent<MovementComponent>(player->GetID());
            auto& gravComp = entMg.GetComponent<GravityComponent>(player->GetID());
            auto& collComp = entMg.GetComponent<CollisionComponent>(player->GetID());
            // Left/right keypress
            if(playerInput.keydown[Key::Left] && playerInput.keydown[Key::Right]) {    // Simultaneous left/right commands cancel out
                playerStop(moveComp.velocity);
            }
            else if(playerInput.keydown[Key::Left]) {  // Move left
                bool aerial = (collComp.onGround == 0);
                if(collComp.onLeftWall != 0) playerStop(moveComp.velocity);
                else playerMove(moveComp.velocity, false, aerial);
            }
            else if(playerInput.keydown[Key::Right]) {  // Move right
                bool aerial = (collComp.onGround == 0);
                if(collComp.onRightWall != 0) playerStop(moveComp.velocity);
                else playerMove(moveComp.velocity, true, aerial);
            }
            else if(playerInput.lastKeydown[Key::Left] && playerInput.lastKeydown[Key::Right]) {   // Movement keyRelease event
                playerStop(moveComp.velocity);
            }
            // Jump keypress
            if(playerInput.keydown[Key::Z] & ~playerInput.lastKeydown[Key::Z]) {   // New keypress
                playerJump();
            }
            else if(~playerInput.keydown[Key::Z] & playerInput.lastKeydown[Key::Z]) {
                playerFall();
            }
            // std::cout << '\n';
        }
        return kContinue; 
    }
private:
    void playerMove(Point2D& velocity, bool right, bool aerial) {
        float accel = kGroundRunAcceleration;
        if(aerial) accel = kAirRunAcceleration;
        if(!right) accel *= -1.0f;
        velocity.x += accel;
    }
    void playerStop(Point2D& velocity) {
        velocity.x = 0;
    }
    void playerJump() {   // TODO: implement variable jump height

    }
    void playerFall() {

    }
};

}  // namespace ECSEngine
