#include <fstream>
#include <stdexcept>

#include "CameraSystem.h"
#include "CollisionComponent.h"
#include "CollisionLateSystem.h"
#include "CollisionSystem.h"
#include "ECSEngine.h"
#include "EntityManager.h"
#include "GravitySystem.h"
#include "InputComponent.h"
#include "InputSystem.h"
#include "LocationComponent.h"
#include "MathUtil.h"
#include "MovementSystem.h"
#include "ScoreComponent.h"
#include "ScoreSystem.h"
#include "SpawnComponent.h"
#include "SpawnSystem.h"
#include "SpriteComponent.h"
#include "SpriteManager.h"
#include "SpriteSystem.h"
#include "MapParser.h"

std::string gResourcePath = "../assets/";
#define COMPONENTS                                                                             \
    ECSEngine::LocationComponent, ECSEngine::MovementComponent, ECSEngine::CollisionComponent, \
        ECSEngine::SpriteComponent, ECSEngine::SpawnComponent, ECSEngine::CameraComponent,     \
        ECSEngine::CameraFollower, ECSEngine::InputComponent, ECSEngine::GravityComponent,     \
        ECSEngine::CameraShake, ECSEngine::ScoreComponent, ECSEngine::RespawnComponent
using Engine = ECSEngine::ECSEngine<COMPONENTS>;

void LoadMap(Engine& engine, const std::string& path, const std::string& file) {}

void makePlayer(Engine& engine, ECSEngine::EntityManager<COMPONENTS>& entityManager, ECSEngine::SpriteManager& spriteManager) {

}

int main(int argc, char* argv[]) {
    if (argc >= 3 && argv[1] == std::string("-path")) {
        gResourcePath = argv[2];
    }
    std::cout << "Usage: " << argv[0] << " [-path resource_path]\n";
    std::cout << "Using resource path: " << gResourcePath << "\n";

    // Engine test code
    Engine engine(1024, 768, "Test Engine");

    ECSEngine::EntityManager<COMPONENTS>& entityManager = engine.GetEntityManager();
    // Load sounds
    auto& soundManager = engine.GetSoundManager();
    for (std::string name : {"error_008", "footstep_grass_003", "footstep_snow_001",
        "laserLarge_000", "sfx_gem", "sfx_jump", "sfx_jump-high"}) {
        soundManager.RegisterSound(gResourcePath + name + ".ogg", name);
    }

    // Load maps
    ECSEngine::MapData background;
    ECSEngine::MapData foreground;

    if (!ECSEngine::ParseMap(gResourcePath + "sky.map", background)) {
        throw std::runtime_error("Failed to load background map.");
        return 1;
    }
    if (!ECSEngine::ParseMap(gResourcePath + "world.map", foreground)) {
        throw std::runtime_error("Failed to load foreground map.");
        return 1;
    }

    // Register parsed texture information into sprites
    ECSEngine::SpriteManager& spriteManager = engine.GetSpriteManager();
    for (auto& it: background.entries) {
        it.second.id = spriteManager.RegisterTexture(it.second.path, it.second.source);
    }
    for (auto& it: foreground.entries) {
        it.second.id = spriteManager.RegisterTexture(it.second.path, it.second.source);
    }

    //Makes the entites for the background and adds the components
    {
        int rowsY = 0;
        for (auto it: background.rows) {
            int rowsX = 0;
            for (char c : it) {

                std::string name = "tile" + std::to_string(rowsX) + "x" + std::to_string(rowsY);
                auto tile = entityManager.CreateEntity(name);
                entityManager.AddComponent<ECSEngine::LocationComponent>(tile, ECSEngine::LocationComponent({background.otlx + rowsX * background.ow, background.otly + rowsY * background.oh}));

                ECSEngine::EntityID texID;
                for (auto &e : background.entries) {
                    if (c == e.first) {
                        texID = e.second.id;
                    }
                }

                entityManager.AddComponent<ECSEngine::SpriteComponent>(tile, ECSEngine::SpriteComponent(texID, ECSEngine::Rect({0,0}, background.ow, background.oh), true));

                rowsX++;
            }
            rowsY++;
        }
    }

    //Foreground creation
    {
        int rowsY = 0;
        for (auto it: foreground.rows) {
            int rowsX = 0;
            for (char c : it) {
                if (c == '.') {
                    rowsX++;
                    continue;
                }
                std::string name = "tile" + std::to_string(rowsX) + "x" + std::to_string(rowsY);
                auto tile = entityManager.CreateEntity(name);
                entityManager.AddComponent<ECSEngine::LocationComponent>(tile, ECSEngine::LocationComponent({foreground.otlx + rowsX * foreground.ow, foreground.otly + rowsY * foreground.oh}));

                ECSEngine::EntityID texID;
                ECSEngine::CollisionComponent colision{};
                colision.doesDamage = 0;

                for (auto &e : foreground.entries) {
                    if (c == e.first) {
                        texID = e.second.id;
                        //Sets the entities colision box
                        colision.collisionBox = e.second.bounds;
                        colision.previousCollisionBox = e.second.bounds;
                    }
                }

                entityManager.AddComponent<ECSEngine::SpriteComponent>(tile, ECSEngine::SpriteComponent(texID, ECSEngine::Rect({0,0}, foreground.ow, foreground.oh), true));

                entityManager.AddComponent<ECSEngine::CollisionComponent>(tile, colision);

                rowsX++;
            }
            rowsY++;
        }
    }
    // Place player
    makePlayer(engine, entityManager, spriteManager);
    //

    // Setup score component
    ECSEngine::ScoreComponent score{};
    score.score = 0;

    for (int i = 0; i < 10; i++) {
        score.digitTextureID[i] = spriteManager.RegisterTexture(
            gResourcePath + "spritesheet-tiles-default.png",
            ECSEngine::Rect(ECSEngine::Point2D(832, 832 - 64 * i), 64.0f, 64.0f)
        );
    }
    for (int i = 0; i < 3; i ++) {
        std::string scoreName = "ScoreEntity" + std::to_string(i);
        score.digitID[i] = entityManager.CreateEntity(scoreName);
        entityManager.AddComponent<ECSEngine::SpriteComponent>(score.digitID[i], ECSEngine::SpriteComponent(score.digitTextureID[0], ECSEngine::Rect({0, 0}, 64, 64), false));
        //Get sprite set location
        entityManager.AddComponent<ECSEngine::LocationComponent>(score.digitID[i], ECSEngine::LocationComponent(ECSEngine::Point2D({0 + 40 * i, 0})));
    }

    auto scoreEntity = entityManager.CreateEntity("Score");
    entityManager.AddComponent<ECSEngine::ScoreComponent>(scoreEntity, score);

    // Setup Camera
    auto cameraEntity = entityManager.CreateEntity("Camera");
    ECSEngine::CameraComponent camera{};
    //TODO: THIS IS JUST TEMP VALUE MAKE PROPER LATER
    camera.position = ECSEngine::Point2D(0, 768/2);
    camera.speed = 5;
    ECSEngine::EntityID playerID = 12345678;
    entityManager.AddComponent<ECSEngine::CameraComponent>(cameraEntity, camera);
    entityManager.AddComponent<ECSEngine::CameraFollower>(cameraEntity, ECSEngine::CameraFollower(playerID));

    // Add all systems
    auto& sm = engine.GetSystemManager();
    sm.AddSystem(std::make_unique<ECSEngine::CollisionLateSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::InputSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::GravitySystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::MovementSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::CollisionSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::ScoreSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::CameraSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::SpriteSystem<COMPONENTS>>());
    sm.AddSystem(std::make_unique<ECSEngine::SpawnSystem<COMPONENTS>>());

    engine.Run();
    return 0;
}
