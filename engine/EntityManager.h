#pragma once

#include <cassert>
#include <iostream>
#include <limits>
#include <type_traits>
#include <vector>
#include <tuple>
#include <optional>
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
     * @returns The generated and assigned EntityID of the created object
     */
    [[nodiscard]] EntityID CreateEntity(const std::string& name) {
        // Create Entity
        const auto currentSize = mEntities.size();
        Entity newEntity(nextID, name);
        assert(newEntity.GetID() == nextID);    // Confirm Entity was created
        nextID++;

        // Add to list
        mEntities.emplace_back(newEntity);
        assert(currentSize + 1 == mEntities.size()); // Confirm Entity was added to vector

        return newEntity.GetID();
    }

    /**
     * @brief Removes and deallocates a provided entity from the list
     * 
     * @param entity The id of the Entity object selected to be removed
     */
    void RemoveEntity(EntityID entity) {
        // Retrieve an iterator for entity (missing entities already checked)
        iterator it = GetEntity(entity);

        // Remove all components from the entity
        (StripComponent<Components>(entity), ...);

        // Erase entity
        const auto currentSize = mEntities.size();
        mEntities.erase(it);
        assert(currentSize - 1 == mEntities.size());    // Confirm Entity was deleted from vector
    }

    /**
     * @brief Assigns a Component to a managed entity
     * 
     * @param entity The id of the Entity object selected
     * @param component A component to assign to the entity
     */
    template <typename T>
    void AddComponent(EntityID entity, T component) {
        assert(!HasComponent<T>(entity));  // Provided entity should not have this component assigned yet

        // Get component storage
        auto& cStore = GetComponentStorage<T>();

        // Update component storage with new values for entry
        size_t index;
        if(cStore.mFreeList.empty()) {
            index = cStore.mStorage.size();
            cStore.mapToEntity.push_back(entity);
            cStore.mValid.push_back(true);
            cStore.mStorage.push_back(component);
        }
        else {
            index = cStore.mFreeList.back();
            cStore.mFreeList.pop_back();
            cStore.mapToEntity[index] = entity;
            cStore.mValid[index] = true;
            cStore.mStorage[index] = component;
        }

        // Map EntityID to storage index
        if(cStore.mapToStorage.size() <= entity) {
            cStore.mapToStorage.resize(entity+1,kNullComponent);
        }
        cStore.mapToStorage[entity] = index;

        assert(HasComponent<T>(entity));    // Confirm component has been added
    }

    /**
     * @brief Clears assignment of an entity to a component
     * 
     * @param entity The id of the Entity object selected
     */
    template <typename T>
    void RemoveComponent(EntityID entity) {
        assert(HasComponent<T>(entity));  // Provided entity should have this component assigned

        // Get component storage
        auto& cStore = GetComponentStorage<T>();

        // Get entity index for component
        size_t index = cStore.mapToStorage[entity];

        // Free elements at index in storage
        cStore.mValid[index] = false;
        cStore.mapToEntity[index] = kNullComponent;
        cStore.mapToStorage[entity] = kNullComponent;
        cStore.mFreeList.push_back(index);

        assert(!HasComponent<T>(entity));    // Confirm component has been removed
    }

    /**
     * @brief Determines if a component has been assigned to a managed entity
     * 
     * @param entity The id of the Entity object selected
     * @returns true if component has been assigned to entity
     */
    template <typename T>
    bool HasComponent(EntityID entity) const {
        // Get component storage
        const auto& cStore = GetComponentStorage<T>();
        
        // Check if the component storage has an index mapped to entity
        if(cStore.mapToStorage.size() <= entity) {
            return false;
        }
        const size_t index = cStore.mapToStorage[entity];
        return index != kNullComponent && cStore.mValid[index];
    }

    /**
     * @brief Retrieves a component associated to an entity
     * 
     * @param entity  The id of the Entity object selected
     * @returns reference to the component of type T associated to the Entity
     */
    template <typename T>
    T& GetComponent(EntityID entity) {
        assert(HasComponent<T>(entity));  // Provided entity should have this component assigned

        // Get component storage
        auto& cStore = GetComponentStorage<T>();

        // Get storage index for entity's component
        size_t index = cStore.mapToStorage[entity];

        // return a reference to the component
        return cStore.mStorage[index];
    }

    /**
     * @brief Retrieves the ComponentStorage object for component T
     * @returns reference to the ComponentStorage<T> object in mComponents
     */
    template <typename T>
    ComponentStorage<T>& GetComponentStorage() {
        return std::get<ComponentStorage<T>>(mComponents);
    }

    template <typename T>
    const ComponentStorage<T>& GetComponentStorage() const {
        return std::get<ComponentStorage<T>>(mComponents);
    }

    // *Taken from template
    using tEntity = std::vector<Entity>;
    using const_iterator = tEntity::const_iterator;
    using iterator = tEntity::iterator;
    const_iterator cbegin() const { return mEntities.begin(); }
    const_iterator cend() const { return mEntities.end(); }
    iterator begin() { return mEntities.begin(); }
    iterator end() { return mEntities.end(); }
    // *

    /**
     * @brief Retrieves an Entity object by searching for id
     *
     * @param entity The id of the Entity object selected
     *
     * @returns an iterator pointing to the Entity in mEntities
     */
    iterator GetEntity(EntityID entity) {
        assert(entity >= 0); // Confirm valid id collection

        // Lambda expression to match target id to entity ids in storage vector
        iterator it = std::find_if(begin(),end(), [entity](const auto& entry) {
            return (entry.GetID() == entity);
        });

        assert(it != end());    // Provided entity should exist within mEntities

        return it;
    }

    /**
     * @brief Retrieves an Entity object by searching for id
     * 
     * @param entity The id of the Entity object selected
     * 
     * @returns a const_iterator pointing to the Entity in mEntities
     */
    const_iterator cGetEntity(EntityID entity) {
        assert(entity >= 0); // Confirm valid id collection

        // Lambda expression to match target id to entity ids in storage vector
        const_iterator it = std::find_if(cbegin(),cend(), [entity](const auto& entry) {
            return (entry.GetID() == entity);
        });

        assert(it != cend());    // Provided entity should exist within mEntities

        return it;
    }

 //    // *Taken from template
 //    using tEntity = std::vector<Entity>;
	// using const_iterator = tEntity::const_iterator;
 //    using iterator = tEntity::iterator;
	// const_iterator cbegin() const { return mEntities.begin(); }
	// const_iterator cend() const { return mEntities.end(); }
	// iterator begin() { return mEntities.begin(); }
	// iterator end() { return mEntities.end(); }
 //    // *

private:
    /**
     * @brief Helper function for RemoveEntity that checks for and strips a component before removal
     * 
     * @param entity The id of the Entity object selected
     */
    template <typename T>
    void StripComponent(EntityID entity) {
        if(HasComponent<T>(entity)) {
            RemoveComponent<T>(entity);
        }
    }
    
    tEntity mEntities;
    EntityID nextID = 0;
    std::tuple<ComponentStorage<Components>...> mComponents;
};

}  // namespace ECSEngine
