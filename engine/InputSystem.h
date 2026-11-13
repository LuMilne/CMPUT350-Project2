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
        playerInput.keydown.reset();
        // Poll for key events in window
        while(std::optional<sf::Event> event = window->pollEvent()) {
            // Moved window close event handling from ECSEngine.h to here
            if (event->is<sf::Event::Closed>()) {
                window->close();
            }
            // Populate InputComponent bitsets
            else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {    // Keypress events
                std::cout << "key pressed!\n";
                playerInput.keydown[static_cast<std::size_t>(keyPressed->scancode)] = 1;
            }
        }
        // Process key events (keys pressed or new key released)
        if(playerInput.keydown.any() || (~playerInput.keydown & playerInput.lastKeydown).any()) {
            // Left/right keypress
            if(playerInput.keydown[Key::Left] && playerInput.keydown[Key::Right]) {    // Simultaneous left/right commands cancel out
                // std::cout << "L+R pressed, ";
                playerStop();
            }
            else if(playerInput.keydown[Key::Left]) {  // Move left
                // std::cout << "L pressed, ";
                playerMove();
            }
            else if(playerInput.keydown[Key::Right]) {  // Move right
                // std::cout << "R pressed, ";
                playerMove();
            }
            else if(playerInput.lastKeydown[Key::Left] && playerInput.lastKeydown[Key::Right]) {   // Movement keyRelease event
                // std::cout << "L/R released, ";
                playerStop();
            }
            // Jump keypress
            if(playerInput.keydown[Key::Z] & ~playerInput.lastKeydown[Key::Z]) {   // New keypress
                // std::cout << "Jump pressed, ";
                playerJump();
            }
            else if(~playerInput.keydown[Key::Z] & playerInput.lastKeydown[Key::Z]) {
                // std::cout << "Jump released, ";
                playerFall();
            }
            // std::cout << '\n';
        }
        return kContinue; 
    }
private:
    void playerMove() {   // TODO: Implement acceleration in horizontal movement (slower accel when airborne)
        
    }
    void playerStop() {
        
    }
    void playerJump() {   // TODO: implement variable jump height

    }
    void playerFall() {

    }
    float groundAccel = 4;
    float airAccel = 3;
};

}  // namespace ECSEngine
