#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

// Central home for every game asset: textures, fonts, and music (more asset
// types can be added the same way - one map plus a load/get pair). Everything
// else (states, Player, Monster, Heart, ...) only ever retrieves already-
// loaded assets through the getX() methods below, never loads anything
// itself.
//
// Loading itself happens in two phases, not one big loadAllAssets() call:
// loadBootAssets() loads just the loading screen's own two images,
// synchronously, cheaply enough to not delay the first frame; everything
// else is queued by queueRemainingAssets() and drained one asset per call
// to loadNext() - see LoadingState, which calls that once per frame so the
// window keeps drawing (and its spinner keeps turning) while the rest of
// the game's assets load, instead of the whole game blocking on one call
// before the window ever shows anything.
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

    // Loads only the loading screen's own background + spinner. Called once
    // from Controller's constructor, before any state exists - small enough
    // (two images) to do synchronously without delaying the first frame.
    void loadBootAssets();

    // Queues every other asset the game needs (fonts, music, every menu/
    // button/monster/attack texture) instead of loading them immediately.
    // Called once, by LoadingState's constructor.
    void queueRemainingAssets();

    // Loads exactly one queued asset (the next one, in the order queued).
    // Returns false once nothing is left to load - the signal for whichever
    // state is driving this (LoadingState) to stop calling it and move on.
    bool loadNext();

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

    // One entry in the deferred-loading queue (see queueRemainingAssets/
    // loadNext above) - just enough to know which loadX() to call and with
    // what arguments once its turn comes.
    struct PendingAsset
    {
        enum class Kind { Texture, Font, Music };
        Kind kind;
        std::string name;
        std::string filePath;
    };

    std::vector<PendingAsset> m_pendingAssets;
    std::size_t m_nextPendingIndex = 0;

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> m_fonts;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> m_music;
};
