#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>
#include <type_traits>

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

    // טעינת נכסים ראשוניים למסך הטעינה בלבד (בלוקינג קצר)
    void loadBootAssets();

    // הכנסת כל שאר נכסי המשחק לתור הטעינה
    void queueRemainingAssets();

    // טעינת הנכס הבא בתור (נקרא בכל פריים מתוך ה-LoadingState)
    bool loadNext();

    // --- 1. מתודת טעינה תבניתית גנרית ---
    template <typename T>
    void load(const std::string& name, const std::string& filePath)
    {
        auto& map = getMap<T>();
        if (map.find(name) != map.end())
            return;

        auto asset = std::make_unique<T>();
        bool success = false;

        // בדיקה בזמן קומפילציה איזו מתודת טעינה להפעיל מ-SFML
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

    // --- 2. מתודות שליפה תבניות גנריות (get<T>) ---
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

    // --- 3. מעטפות נוחות לשמירה על תאימות עם הקוד הקיים בפרויקט ---
    const sf::Texture& getTexture(const std::string& name) const { return get<sf::Texture>(name); }
    const sf::Font& getFont(const std::string& name) const {
		// בא לי שאם לא מצאנו את הפונט שביקושנו, ננסה להחזיר את הפונט ברירת המחדל (DefaultFont) במקום לזרוק חריגה. זה יכול להיות שימושי במצבים שבהם הפונט לא קריטי, או כשאנחנו רוצים להבטיח שהמשחק ימשיך לרוץ גם אם נכס מסוים חסר.
        return get<sf::Font>(name);
    }
    sf::Music& getMusic(const std::string& name) const { return const_cast<sf::Music&>(get<sf::Music>(name)); }
    const sf::SoundBuffer& getSoundBuffer(const std::string& name) const { return get<sf::SoundBuffer>(name); }

private:
    AssetsManager() = default;

    struct PendingAsset
    {
        enum class Kind { Texture, Font, Music, SoundBuffer };
        Kind kind;
        std::string name;
        std::string filePath;
    };

    // --- 4. מיפוי תבניתי פנימי (מיפוי T למפה המתאימה ב-AssetsManager) ---
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
    std::size_t m_nextPendingIndex = 0;

    // המפות שמחזיקות את הנכסים בזיכרון
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>>     m_textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>>        m_fonts;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>>       m_music;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> m_soundBuffers;
};