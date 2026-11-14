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
        auto& inputStorage = entMg.template GetComponentStorage<InputComponent>();
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
        auto& gravComp = entMg.template GetComponent<GravityComponent>(player->GetID());
        if (gravComp.disabledTimer != 0) {
            return kContinue;
        }
        // Process key events (keys pressed or new key released)
        if(playerInput.keydown.any() || (~playerInput.keydown & playerInput.lastKeydown).any()) {
            auto& collComp = entMg.template GetComponent<CollisionComponent>(player->GetID());
            auto& moveComp = entMg.template GetComponent<MovementComponent>(player->GetID());
            auto& soundMg = engine->GetSoundManager();

            // Left/right keypress
            if(playerInput.keydown[Key::Left] && playerInput.keydown[Key::Right]) {    // Simultaneous left/right commands cancel out
                playerStop(moveComp.velocity);
            }
            else if(playerInput.keydown[Key::Left]) {  // Move left
                bool aerial = (collComp.onGround == 0);
                playerMove(moveComp.velocity, false, aerial);
            }
            else if(playerInput.keydown[Key::Right]) {  // Move right
                bool aerial = (collComp.onGround == 0);
                playerMove(moveComp.velocity, true, aerial);
            }
            else {   // Movement keyRelease event
                playerStop(moveComp.velocity);
            }
            // Jump keypress
            if(playerInput.keydown[Key::Z] & ~playerInput.lastKeydown[Key::Z]) {   // New keypress
                if (collComp.onLeftWall == 1 && collComp.onGround == 0) {
                    playerJump(soundMg, moveComp.velocity);
                    moveComp.velocity.x = 30;
                    playerMove(moveComp.velocity, false, true);
                } else if (collComp.onRightWall == 1 && collComp.onGround == 0) {
                    playerJump(soundMg, moveComp.velocity);
                    moveComp.velocity.x = -30;
                    playerMove(moveComp.velocity, true, true);
                }
                if(collComp.onGround != 0) playerJump(soundMg, moveComp.velocity);
            }
            else if(~playerInput.keydown[Key::Z] & playerInput.lastKeydown[Key::Z]) {
                playerFall(moveComp.velocity);
            }
            // std::cout << '\n';
            if(playerInput.keydown[Key::Down]) {  // Move Down Faster
                bool aerial = (collComp.onGround == 0);
                if (aerial) {
                    playerFallFast(moveComp.velocity);
                }
            }

            if (playerInput.keydown[Key::C] & ~playerInput.lastKeydown[Key::C]) {
                if (playerInput.lastDash == 0) {
                    if ((collComp.onRightWall == 1 && collComp.onGround == 0) || (moveComp.velocity.x < 0 && collComp.onLeftWall == 0)) {
                        float dir = -1;
                        gravComp.disabledTimer = int(kDashInterval);
                        playerInput.lastDash = 60;
                        playerDash(soundMg, moveComp.velocity, dir);
                    } else if ((collComp.onLeftWall == 1 && collComp.onGround == 0) || (moveComp.velocity.x > 0 && collComp.onRightWall == 0)) {
                        float dir = 1;
                        gravComp.disabledTimer = int(kDashInterval);
                        playerInput.lastDash = 60;
                        playerDash(soundMg, moveComp.velocity, dir);
                    }
                }
            }

        }

        if (playerInput.lastDash != 0) {playerInput.lastDash -= 1;}

        return kContinue; 
    }
private:
    void playerMove(Point2D& velocity, bool right, bool aerial) {
        float accel = kGroundRunAcceleration;
        if(aerial) accel = kAirRunAcceleration;
        if(!right) accel *= -1.0f;
        velocity.x = std::clamp(velocity.x + accel, -kMaxRunVelocity, kMaxRunVelocity);
    }
    void playerStop(Point2D& velocity) {
        velocity.x = 0;
    }
    void playerJump(SoundManager& soundMg, Point2D& vel) {
        auto& jumpSound = soundMg.GetSound("sfx_jump");
        jumpSound.play();
        vel.y += -kJumpVelocity;
        vel.y = std::clamp(vel.y, -kJumpVelocity, kJumpVelocity);
    }
    void playerFall(Point2D& vel) {
        if (vel.y < 0) {
            vel.y = 0;
        }
    }

    void playerFallFast(Point2D& vel) {
        vel.y += 5;
    }

    void playerDash(SoundManager& soundMg, Point2D& vel, float dir) {
        auto& dashSound = soundMg.GetSound("laserLarge_000");
        dashSound.play();
        vel.x = kDashVelocity * dir;
    }
    float groundAccel = 4;
    float airAccel = 3;
};

}  // namespace ECSEngine
