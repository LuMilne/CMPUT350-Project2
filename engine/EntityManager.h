#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <type_traits>
#include <vector>
#include <tuple>
#include <algorithm>

#include "Component.h"
#include "Entity.h"

namespace ECSEngine {

/**
 * @brief Maintains list of active Entities and their Components
 * 
 * This class handles the storage and retreival of Entity objects and the assignment of Component objects to them.
 */
template <typename... Components>
class EntityManager {
public:
    static constexpr size_t kNullComponent = std::numeric_limits<size_t>::max();

    /**
     * @brief Creates a new Entity object and adds it to the list of managed Entities
     * 
     * @param name The name of the Entity object
     * @return The generated and assigned EntityID of the created object
     */
    [[nodiscard]] EntityID CreateEntity(const std::string& name) { return 0; }

    /**
     * @brief Removes and deallocates a provided entity from the list
     * 
     * @param entity The Entity object selected to be removed
     * @throws std::runtime_error if entity is not found in the list
     */
    void RemoveEntity(EntityID entity) { return; }

    /**
     * @brief Assigns a Component to a managed entity
     * 
     * @param entity The entity object selected for the component
     * @param component A component to assign to the entity
     * @throws std::runtime_error if entity is not found in the list
     */
    template <typename T>
    void AddComponent(EntityID entity, T component) {}

    template <typename T>
    void RemoveComponent(EntityID entity) {}

    template <typename T>
    bool HasComponent(EntityID entity) const {
        return false;
    }

    template <typename T>
    T& GetComponent(EntityID entity) {
        // Re-write code
        static T dummy;
        return dummy;
    }

    template <typename T>
    ComponentStorage<T>& GetComponentStorage() {
        // Re-write code
        static ComponentStorage<T> dummy;
        return dummy;
    }

    // Entity Managing
    std::vector<tuple<Entity,Components>> mEntities;
    EntityID nextID = 1;
    // *Added from template
    using tEntity = std::vector<Entity>;
	using const_iterator = tEntity::const_iterator;
	const_iterator cbegin() const { return mEntities.begin(); }
	const_iterator cend() const { return mEntities.end(); }
	const_iterator begin() { return mEntities.begin(); }
	const_iterator end() { return mEntities.end(); }
    // *

private:
};

}  // namespace ECSEngine
