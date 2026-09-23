#pragma once
#include "Button.h"
#include "AssetsManager.h"
#include "MusicManager.h"
#include "SoundPlayer.h"
#include <string>
#include <utility>
#include <type_traits>

// A mute/unmute icon Button generic over which audio service it toggles
// (MusicManager or SoundPlayer) - see the aliases below. Reads/writes that
// service's mute flag, so its icon always stays in sync with actual playback.
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

    // Re-reads the mute flag and refreshes the icon every call, so a toggle
    // clicked on one screen stays in sync everywhere this button is drawn.
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
    // Default unmuted/muted texture names, picked by which AudioService this
    // instance is specialized for.
    static std::pair<const char*, const char*> getDefaultTextures()
    {
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

        // Confirms unmuting with a click sound - only for SoundPlayer, so
        // toggling music doesn't also play a sound effect.
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