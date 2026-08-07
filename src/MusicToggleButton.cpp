#include "MusicToggleButton.h"
#include "AssetsManager.h"
#include "MusicManager.h"

namespace
{
    const sf::Texture& currentIconTexture()
    {
        auto& am = AssetsManager::getInstance();
        return am.getTexture(MusicManager::getInstance().isMuted() ? "VolumeMuteButton" : "VolumeUpButton");
    }

    sf::IntRect makeRect(sf::Vector2i position, unsigned int width)
    {
        auto textureSize = currentIconTexture().getSize();
        float scale = static_cast<float>(width) / static_cast<float>(textureSize.x);
        int scaledHeight = static_cast<int>(textureSize.y * scale);
        return sf::IntRect(position, { static_cast<int>(width), scaledHeight });
    }

    sf::Vector2f makeScale(unsigned int width)
    {
        float scale = static_cast<float>(width) / static_cast<float>(currentIconTexture().getSize().x);
        return { scale, scale };
    }
}

MusicToggleButton::MusicToggleButton(sf::Vector2i position, unsigned int width)
    : m_button(makeRect(position, width), currentIconTexture(), [this]() { onClicked(); }, makeScale(width))
{
}

void MusicToggleButton::onClicked()
{
    MusicManager::getInstance().toggleMute();
}

void MusicToggleButton::refreshTexture() const
{
    m_button.setTexture(currentIconTexture());
}

void MusicToggleButton::draw(sf::RenderWindow& window) const
{
    refreshTexture();
    m_button.draw(window);
}

void MusicToggleButton::handleEvent(const sf::Event& event)
{
    m_button.handleEvent(event);
}
