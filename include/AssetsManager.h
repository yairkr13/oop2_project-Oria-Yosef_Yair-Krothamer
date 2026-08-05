#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>

// Central home for every game asset: textures, fonts, and music (more asset
// types can be added the same way - one map plus a load/get pair). Assets
// are all loaded once, up front, via loadAllAssets(); everything else
// (states, Player, Monster, Heart, ...) only ever retrieves already-loaded
// assets through the getX() methods below, never loads anything itself.
class AssetsManager
{
public:
    AssetsManager(const AssetsManager&) = delete;
    AssetsManager& operator=(const AssetsManager&) = delete;

    static AssetsManager& getInstance()
    {
        static AssetsManager instance;
        return instance;
    }

    // Loads every asset the application needs. Called once from Controller,
    // before any state (which retrieves assets in its constructor) exists.
    void loadAllAssets();

    void loadTexture(const std::string& name, const std::string& filePath);
    const sf::Texture& getTexture(const std::string& name) const;

    void loadFont(const std::string& name, const std::string& filePath);
    const sf::Font& getFont(const std::string& name) const;

    void loadMusic(const std::string& name, const std::string& filePath);
    // Non-const on purpose, unlike the getters above: callers need to
    // control playback (play/pause/stop), not just read the asset.
    sf::Music& getMusic(const std::string& name) const;

private:
    AssetsManager() = default;

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> m_fonts;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> m_music;
};
