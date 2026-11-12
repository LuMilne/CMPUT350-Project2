#pragma once

#include <vector>
// #include "ECSEngine.h"

namespace ECSEngine {

enum tSystemReturn { kContinue, kExit };

template <typename... Components>
class EngineView;

template <typename... Components>
class System {
public:
    virtual ~System() = default;
    virtual tSystemReturn Run(EngineView<Components...>* scene) = 0;
    virtual const std::string& GetName() const = 0;
};

template <typename... Components>
class SystemManager {
public:
    bool Run(EngineView<Components...>* scene) {
        std::cout << "SystemManager::Run()\n";
        int temp = 0;
        for (auto& system : mSystems) {
            temp++;
            std::cout << "SystemManager for loop: " << temp << "\n";
            auto response = system->Run(scene);
            if (response == kExit)  // system wants to exit
            {
                std::cout << system->GetName() << " requested exit from system manager\n";
                return false;
            }
        }
        return true;
    }
    void AddSystem(std::unique_ptr<System<Components...>>&& system) {
        mSystems.push_back(std::move(system));
    }
    void ClearSystems() { mSystems.clear(); }

private:
    std::vector<std::unique_ptr<System<Components...>>> mSystems;
};

}  // namespace ECSEngine
