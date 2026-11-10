#include <iostream>
#include <stdexcept>

#include "EntityManager.h"


namespace ECSEngine {
using namespace ECSEngine;

template <typename... Components>
EntityID CreateEntity(const std::string& name){
    // Create Entity
    int currentSize = mEntities.size();
    Entity newEntity = new Entity(nextID, name);
    assert(newEntity.GetID() == nextID);    // Confirm Entity was created
    nextID++;

    // Add to list
    mEntities.push_back();
    assert(currentSize != mEntities.size()) // Confirm Entity was added to vector
    return newEntity.GetID();
}

template <typename... Components>
void RemoveEntity(EntityID entity) {
    assert(entity > 0); // Confirm valid id collection

    // Lambda expression to match target id to entity ids in storage vector
    const_iterator it = std::find_if(begin(),end(), [entity](const auto& obj) {
        return std::get<0>(entry.GetID() == entity);
    });

    // Provided entity should exist within mEntities
    if( it == end() ) { throw std::runtime_error("Attempted to delete Entity not found within manager.") }

    // Erase entity
    int currentSize = mEntities.size();
    mEntities.erase(it);
    assert(currentSize != mEntities.size());    // Confirm Entity was deleted from vector
}

template <typename T>
void AddComponent(EntityID entity, T component) {
    // Lambda expression to match target id to entity ids in storage vector
    const_iterator it = std::find_if(begin(),end(), [entity](const auto& obj) {
        return std::get<0>(entry.GetID() == entity);
    });

    // Provided entitiy should exist within mEntities
    if( it == end() ) { throw std::runtime_error("Entity not found within manager.") }
}

}   // namespace ECSEngine