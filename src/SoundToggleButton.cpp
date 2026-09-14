//#include "SoundToggleButton.h"
//#include "SoundPlayer.h"
//#include "AssetsManager.h"
//
//SoundToggleButton::SoundToggleButton(sf::Vector2i position, unsigned int width)
//    : m_button(position, width, "VolumeUpButton", [this]() { onClicked(); })
//{
//    refreshTexture();
//}
//
//void SoundToggleButton::draw(sf::RenderWindow& window) const
//{
//    refreshTexture(); // סנכרון התמונה מול ה-SoundPlayer
//    m_button.draw(window);
//}
//
//void SoundToggleButton::handleEvent(const sf::Event& event)
//{
//    m_button.handleEvent(event);
//}
//
//void SoundToggleButton::onClicked()
//{
//    SoundPlayer::getInstance().toggleMute();
//    refreshTexture();
//}
//
//void SoundToggleButton::refreshTexture() const
//{
//    bool muted = SoundPlayer::getInstance().isMuted();
//    std::string textureKey = muted ? "VolumeMuteButton" : "VolumeUpButton";
//    m_button.setTexture(AssetsManager::getInstance().getTexture(textureKey));
//}