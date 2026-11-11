#pragma once

#include <unordered_map>
#include <string>

#include "MathUtil.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace ECSEngine {

using SpriteID = char;

class SpriteManager {
public:
    SpriteManager() = default;
    ~SpriteManager() = default;
    /**
     * @brief Creates a sprite from the provided texture and stores in mSprites
     * 
     * @param id a unique character, taken from the texture maps
     * @param texturePath the source file for the texture
     * @param sourceRect the coordinates and dimensions of the texture in the file
     * @throws runtime_error if texture fails to load from texturePath
     * @returns the ID of the sprite
     */
    [[nodiscard]] SpriteID RegisterTexture(const char id, const std::string& texturePath, const Rect& sourceRect);

    /**
     * @brief Returns a stored sprite
     * 
     * @param id a unique character, taken from the texture maps
     * @throws runtime_error if a sprite is not found at 'id'
     * @returns a reference to a Sprite object
     */
    sf::Sprite& GetSprite(SpriteID id);

private:
    std::unordered_map<SpriteID,sf::Texture>  mTextures;
    std::unordered_map<SpriteID,sf::Sprite>  mSprites;
    SpriteID nextID = 0;
};

}  // namespace ECSEngine
