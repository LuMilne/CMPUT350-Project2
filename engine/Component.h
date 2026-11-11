#pragma once

namespace ECSEngine {

template <typename T>
class ComponentStorage {
public:
    std::vector<T> mStorage;
    std::vector<bool> mValid;           // Quickly track if a component is valid
    std::vector<size_t> mFreeList;      // Avoid scanning for free slots
    std::vector<size_t> mapToStorage;   // Tracks which storage index is associated with the corresponding EntityID
    std::vector<size_t> mapToEntity;    // Tracks which EntityID is associated with the corresponding storage index
};

}  // namespace ECSEngine
