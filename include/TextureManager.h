#pragma once
#include <SFML/Graphics.hpp>
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

    void loadTexture(const std::string& name, const std::string& filePath)
    {
        if (m_textures.find(name) == m_textures.end())
        {
            auto tex = std::make_unique<sf::Texture>();
            if (!tex->loadFromFile(filePath)) // עכשיו קוראים מהנתיב המלא
            {
                throw std::runtime_error("Failed to load texture: " + filePath);
            }
            m_textures[name] = std::move(tex);
        }
    }

    const sf::Texture& getTexture(const std::string& name) const
    {
        return *m_textures.at(name);
    }

private:
    TextureManager() = default;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
};
