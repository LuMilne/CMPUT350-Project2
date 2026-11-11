#include <stdexcept>
#include <utility>

#include "SpriteManager.h"

namespace ECSEngine {

SpriteID SpriteManager::RegisterTexture(const char id, const std::string& texturePath, const Rect& sourceRect) {
    // Load subtexture
    sf::Texture subtexture;
    bool load = subtexture.loadFromFile(
        texturePath, 
        false, 
        sf::IntRect(
            {sourceRect.topLeft.x, sourceRect.topLeft.y}, 
            {sourceRect.width, sourceRect.height}
        )
    );
    if(!load) { // Verify that subtexture was loaded successfully
        throw std::runtime_error("Failed to load " + texturePath);
    }
    
    size_t currentSize = mSprites.size();
    // Add texture to mTextures
    mTextures.insert({id, std::move(subtexture)});
    // Construct a sprite and add to mSprites
    mSprites.insert({id, sf::Sprite(mTextures.find(id)->second)});
    assert(currentSize + 1 == mSprites.size() && currentSize + 1 == mTextures.size()); // Verify that element was added
    return id;
}

sf::Sprite& SpriteManager::GetSprite(SpriteID id) {
    const auto it = mSprites.find(id);
    if(it == mSprites.end()) { // Verify that mSprites has a sprite at 'id'
        throw std::runtime_error("Sprite not found");
    }
    return it->second;
}

}  // namespace ECSEngine
