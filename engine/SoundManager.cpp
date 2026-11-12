#include "SoundManager.h"

namespace ECSEngine {

void SoundManager::RegisterSound(const std::string& soundPath, const std::string& soundName) {
    auto& stored = mBuffer[soundName];
    if (stored.loadFromFile(soundPath)) {
        mSound.try_emplace(soundPath, stored);
    }

}

sf::Sound& SoundManager::GetSound(const std::string& soundName) {
    // static sf::SoundBuffer dummyBuffer;
    // static sf::Sound dummy(dummyBuffer);
    // return dummy;
    return mSound.at(soundName);
}

}  // namespace ECSEngine
