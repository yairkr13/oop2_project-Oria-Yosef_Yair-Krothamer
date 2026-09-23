//#pragma once
//#include <SFML/Graphics.hpp>
//#include <SFML/Audio.hpp>
//#include <unordered_map>
//#include <string>
//#include <memory>
//#include <vector>
//
//// Central home for every game asset: textures, fonts, and music (more asset
//// types can be added the same way - one map plus a load/get pair). Everything
//// else (states, Player, Monster, Heart, ...) only ever retrieves already-
//// loaded assets through the getX() methods below, never loads anything
//// itself.
////
//// Loading itself happens in two phases, not one big loadAllAssets() call:
//// loadBootAssets() loads just the loading screen's own two images,
//// synchronously, cheaply enough to not delay the first frame; everything
//// else is queued by queueRemainingAssets() and drained one asset per call
//// to loadNext() - see LoadingState, which calls that once per frame so the
//// window keeps drawing (and its spinner keeps turning) while the rest of
//// the game's assets load, instead of the whole game blocking on one call
//// before the window ever shows anything.
//class AssetsManager //להפוך את זה לעם פונקציות תבניתיות!!!!!!
//    //אם כל הget הם אותו דבר למה לא להחליף אותם בפונקציה התבניתית וזהו????
//{
//public:
//    AssetsManager(const AssetsManager&) = delete;
//    AssetsManager& operator=(const AssetsManager&) = delete;
//
//    static AssetsManager& getInstance()
//    {
//        static AssetsManager instance;
//        return instance;
//    }
//
//    // Loads only the loading screen's own background + spinner. Called once
//    // from Controller's constructor, before any state exists - small enough
//    // (two images) to do synchronously without delaying the first frame.
//    void loadBootAssets();
//
//    // Queues every other asset the game needs (fonts, music, every menu/
//    // button/monster/attack texture) instead of loading them immediately.
//    // Called once, by LoadingState's constructor.
//    void queueRemainingAssets();
//
//    // Loads exactly one queued asset (the next one, in the order queued).
//    // Returns false once nothing is left to load - the signal for whichever
//    // state is driving this (LoadingState) to stop calling it and move on.
//    bool loadNext();
//
//    void loadTexture(const std::string& name, const std::string& filePath);
//    const sf::Texture& getTexture(const std::string& name) const;
//
//    void loadFont(const std::string& name, const std::string& filePath);
//    const sf::Font& getFont(const std::string& name) const;
//
//    void loadMusic(const std::string& name, const std::string& filePath);
//    // Non-const on purpose, unlike the getters above: callers need to
//    // control playback (play/pause/stop), not just read the asset.
//    sf::Music& getMusic(const std::string& name) const;
//
//    //// (אופציונלי) מעטפות נוחות כדי לא לשבור קוד קיים שקורא ל-getTexture וכו'
//    //const sf::Texture& getTexture(const std::string& name) const { return get<sf::Texture>(name); }
//    //const sf::Font& getFont(const std::string& name) const { return get<sf::Font>(name); }
//    //sf::Music& getMusic(const std::string& name) { return get<sf::Music>(name); }
//
//    void loadSoundBuffer(const std::string& name, const std::string& filePath);
//    const sf::SoundBuffer& getSoundBuffer(const std::string& name) const;
//private:
//    AssetsManager() = default;
//
//    // One entry in the deferred-loading queue (see queueRemainingAssets/
//    // loadNext above) - just enough to know which loadX() to call and with
//    // what arguments once its turn comes.
//    struct PendingAsset
//    {
//        enum class Kind { Texture, Font, Music ,SoundBuffer };
//        Kind kind;
//        std::string name;
//        std::string filePath;
//    };
//
//    std::vector<PendingAsset> m_pendingAssets;
//    std::size_t m_nextPendingIndex = 0;
//
//    //לשנות את שני הפונקציות האלה
//    template <typename T>
//    void loadAsset(std::unordered_map<std::string, std::unique_ptr<T>>& map,
//        const std::string& name, const std::string& filePath,
//        bool (T::* openMethod)(const std::string&), const char* typeLabel);
//
//    // Mirrors loadAsset above for the read side: find-or-throw, shared by
//    // every getX() method below (except getMusic, which stays its own
//    // thin non-const wrapper around this - see its comment).
//    template <typename T>
//    const T& getAsset(const std::unordered_map<std::string, std::unique_ptr<T>>& map,
//        const std::string& name, const char* typeLabel) const;
//
//
//    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
//    std::unordered_map<std::string, std::unique_ptr<sf::Font>> m_fonts;
//    std::unordered_map<std::string, std::unique_ptr<sf::Music>> m_music;
//    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> m_soundBuffers;
//};
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

    /*void queueUIAssets();
    void queueMonsterAssets();
    void queueAttackAssets();
    void queueSpecialEffectsAssets();
    void queueAudioAssets();*/

    // Maps T to the member map holding it.
    template <typename T>
    auto& getMap()
    {
        if constexpr (std::is_same_v<T, sf::Texture>)           return m_textures;
        else if constexpr (std::is_same_v<T, sf::Font>)        return m_fonts;
        else if constexpr (std::is_same_v<T, sf::Music>)       return m_music;
        else if constexpr (std::is_same_v<T, sf::SoundBuffer>) return m_soundBuffers;
    }

    template <typename T>
    const auto& getMap() const
    {
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