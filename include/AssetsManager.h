#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <type_traits>

// Central store for every game asset - textures, fonts, music, sound
// buffers. Everything else only ever retrieves already-loaded assets through
// this class; loading is deferred (see queueRemainingAssets/loadNext) so the
// loading screen can keep drawing while the rest streams in.
class AssetsManager
{
public:
    AssetsManager(const AssetsManager&) = delete;
    AssetsManager& operator=(const AssetsManager&) = delete;

    static AssetsManager& getInstance();

    void loadBootAssets();

    void queueRemainingAssets();

    bool loadNext();

    // Generic loader: loads `filePath` as a T under `name`, no-op if already loaded.
    template <typename T>
    void load(const std::string& name, const std::string& filePath)
    {
        auto& map = getMap<T>();
        if (map.find(name) != map.end())
            return;

        auto asset = std::make_unique<T>();
        bool success = false;

        // sf::Texture/sf::SoundBuffer use loadFromFile; sf::Font/sf::Music use openFromFile.
        if constexpr (std::is_same_v<T, sf::Texture> || std::is_same_v<T, sf::SoundBuffer>) {
            success = asset->loadFromFile(filePath);
        }
        else {
            success = asset->openFromFile(filePath);
        }

        if (!success) {
            throw std::runtime_error("AssetsManager: Failed to load asset '" + name + "' from \"" + filePath + "\"");
        }

        map[name] = std::move(asset);
    }

    // Generic lookup, mutable and const overloads - throws if `name` was
    // never loaded.
    template <typename T>
    T& get(const std::string& name)
    {
        auto& map = getMap<T>();
        auto it = map.find(name);
        if (it == map.end()) {
            throw std::out_of_range("AssetsManager: Asset '" + name + "' was never loaded!");
        }
        return *it->second;
    }

    template <typename T>
    const T& get(const std::string& name) const
    {
        const auto& map = getMap<T>();
        auto it = map.find(name);
        if (it == map.end()) {
            throw std::out_of_range("AssetsManager: Asset '" + name + "' was never loaded!");
        }
        return *it->second;
    }

    const sf::Texture& getTexture(const std::string& name) const;
    const sf::Font& getFont(const std::string& name) const;
    sf::Music& getMusic(const std::string& name) const;
    const sf::SoundBuffer& getSoundBuffer(const std::string& name) const;

private:
    AssetsManager() = default;

    struct PendingAsset
    {
        enum class Kind { Texture, Font, Music, SoundBuffer };
        Kind kind;
        std::string name;
        std::string filePath;
    };

    // Maps T to the member map holding it.
    template <typename T>
    auto& getMap()
    {
        // dispatch on the asset type T to pick its backing map - one branch per asset kind
        if constexpr (std::is_same_v<T, sf::Texture>)           return m_textures;
        else if constexpr (std::is_same_v<T, sf::Font>)        return m_fonts;
        else if constexpr (std::is_same_v<T, sf::Music>)       return m_music;
        else if constexpr (std::is_same_v<T, sf::SoundBuffer>) return m_soundBuffers;
    }

    template <typename T>
    const auto& getMap() const
    {
        // same dispatch as the non-const overload above, for const callers
        if constexpr (std::is_same_v<T, sf::Texture>)           return m_textures;
        else if constexpr (std::is_same_v<T, sf::Font>)        return m_fonts;
        else if constexpr (std::is_same_v<T, sf::Music>)       return m_music;
        else if constexpr (std::is_same_v<T, sf::SoundBuffer>) return m_soundBuffers;
    }

    std::vector<PendingAsset> m_pendingAssets;
    std::size_t m_nextPendingIndex = 0; // read position into m_pendingAssets

    std::unordered_map<std::string, std::unique_ptr<sf::Texture>>     m_textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>>        m_fonts;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>>       m_music;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> m_soundBuffers;
};