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

    template <typename T>
    void load(const std::string& name, const std::string& filePath)
    {
        auto& map = getMap<T>(); // הקומפיילר יבחר את המפה הנכונה לפי סוג ה-T

        if (map.find(name) == map.end())
        {
            auto resource = std::make_unique<T>();
            bool success = false;

            // כאן אנחנו מפצלים את סוג הטעינה לפי סוג האובייקט
            if constexpr (std::is_same_v<T, sf::Font>)
            {
                success = resource->openFromFile(filePath); // פונט דורש open
            }
            else
            {
                success = resource->loadFromFile(filePath); // טקסטורה דורשת load
            }

            if (!success)
            {
                throw std::runtime_error("Failed to load resource: " + filePath);
            }

            map[name] = std::move(resource);
        }
    }

    template <typename T>
    const T& get(const std::string& name) const
    {
        return *getMap<T>().at(name);
    }

    //void loadTexture(const std::string& name, const std::string& filePath)
    //{
    //    if (m_textures.find(name) == m_textures.end())
    //    {
    //        auto tex = std::make_unique<sf::Texture>();
    //        if (!tex->loadFromFile(filePath)) // עכשיו קוראים מהנתיב המלא
    //        {
    //            throw std::runtime_error("Failed to load texture: ");
    //        }
    //        m_textures[name] = std::move(tex);
    //    }
    //}

    //const sf::Texture& getTexture(const std::string& name) const
    //{
    //    return *m_textures.at(name);
    //}

private:
    TextureManager() = default;
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> m_fonts; // המפה החדשה לפונטים
    template <typename T>
    auto& getMap()
    {
        if constexpr (std::is_same_v<T, sf::Texture>) return m_textures;
        else if constexpr (std::is_same_v<T, sf::Font>) return m_fonts;
        else static_assert(!sizeof(T*), "Unsupported resource type!");
    }

    // גרסת const של אותה פונקציה בדיוק
    template <typename T>
    const auto& getMap() const
    {
        if constexpr (std::is_same_v<T, sf::Texture>) return m_textures;
        else if constexpr (std::is_same_v<T, sf::Font>) return m_fonts;
        else static_assert(!sizeof(T*), "Unsupported resource type!");
    }
};
