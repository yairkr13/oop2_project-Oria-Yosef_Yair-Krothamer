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
        , m_button(Button::fromTextureWidth(position, currentIconTexture(), width, [this]() { onClicked(); }))
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
        // pick the icon pair matching which service this instance toggles - sound vs. music
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

        // only the sound-toggle button needs this: the click sound Button already played
        // was silent if we were muted, so replay it now that unmuting just turned it audible
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

    std::string m_unmutedTexture;
    std::string m_mutedTexture;

    mutable Button m_button;
};

using MusicToggleButton = AudioToggleButton<MusicManager>;
using SoundToggleButton = AudioToggleButton<SoundPlayer>;