#pragma once
#include "Button.h"
#include "AssetsManager.h"
#include "MusicManager.h"
#include "SoundPlayer.h"
#include <string>
#include <utility>
#include <type_traits>

//זה אבסטרקטית ויחודית?????
template <typename AudioService>
class AudioToggleButton
{
public:
    AudioToggleButton(sf::Vector2i position, unsigned int width,
        std::string unmutedTexture = getDefaultTextures().first,
        std::string mutedTexture = getDefaultTextures().second)
        : m_unmutedTexture(std::move(unmutedTexture))
        , m_mutedTexture(std::move(mutedTexture))
        , m_button(makeRect(position, width), currentIconTexture(), [this]() { onClicked(); }, makeScale(width))
    {
    }

    void draw(sf::RenderWindow& window) const
    {
        refreshTexture();
        m_button.draw(window);
    }

    void handleEvent(const sf::Event& event)
    {
        m_button.handleEvent(event);
    }

private:
    // פונקציה סטטית פנימית שקובעת את טקסטורות ברירת המחדל לפי הטיפול ב-AudioService
    static std::pair<const char*, const char*> getDefaultTextures()
    { //זה בסדר שדבר כזה נמצא בפונקציה תבניתית?????
        if constexpr (std::is_same_v<AudioService, SoundPlayer>)
        {
            return { "SoundUpButton", "SoundMuteButton" };
        }
        else
        {
            return { "VolumeUpButton", "VolumeMuteButton" };
        }
    }

    void onClicked()
    {
        AudioService::getInstance().toggleMute();

        if constexpr (std::is_same_v<AudioService, SoundPlayer>)
        {
            if (!SoundPlayer::getInstance().isMuted())
            {
                SoundPlayer::getInstance().play("button_click");
            }
        }
    }

    void refreshTexture() const
    {
        m_button.setTexture(currentIconTexture());
    }

    const sf::Texture& currentIconTexture() const
    {
        auto& am = AssetsManager::getInstance();
        bool muted = AudioService::getInstance().isMuted();
        return am.getTexture(muted ? m_mutedTexture : m_unmutedTexture);
    }

    sf::IntRect makeRect(sf::Vector2i position, unsigned int width) const
    {
        auto textureSize = currentIconTexture().getSize();
        float scale = static_cast<float>(width) / static_cast<float>(textureSize.x);
        int scaledHeight = static_cast<int>(textureSize.y * scale);
        return sf::IntRect(position, { static_cast<int>(width), scaledHeight });
    }

    sf::Vector2f makeScale(unsigned int width) const
    {
        float scale = static_cast<float>(width) / static_cast<float>(currentIconTexture().getSize().x);
        return { scale, scale };
    }

    std::string m_unmutedTexture;
    std::string m_mutedTexture;

    mutable Button m_button;
};

using MusicToggleButton = AudioToggleButton<MusicManager>;
using SoundToggleButton = AudioToggleButton<SoundPlayer>;