#pragma once
#include "SFML/Graphics.hpp"
#include <unordered_map>
#include <string>
#include <memory>

class TextureManager
{
public:
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    static TextureManager& getInstance()
    {
        static TextureManager instance;
        return instance;
    }
    void loadTexture(const std::string& name, const std::string& filename) 
    {
        if (m_textures.find(name) == m_textures.end()) 
        {
            if (!m_textures[name].loadFromFile(filename)) 
            {
                throw std::runtime_error("Failed to load texture: " + filename);
            }
        }
    }

    const sf::Texture& getTexture(const std::string& name) 
    {
        return m_textures[name];
    }


private:
    TextureManager() {};
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
};
