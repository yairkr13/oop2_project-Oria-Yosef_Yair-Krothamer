#include "AssetsManager.h"
#include <stdexcept>

void AssetsManager::loadAllAssets()
{
    loadFont("arial", "resources/Fonts/arial.ttf");
    loadFont("Lilita", "resources/Fonts/LilitaOne.ttf");

    loadMusic("menu_music", "resources/Music/BackGround/BackGroundMusic.mp3");
    loadMusic("gameplay_music", "resources/Music/BackGround/InGameMusic.mp3");

    loadTexture("menu_bg", "resources/Menu/Menu.png");
    loadTexture("instructions_bg", "resources/Menu/Instructions.png");
    loadTexture("mini_menu_bg", "resources/Menu/MiniMenu.png");
    loadTexture("gameover_bg", "resources/Menu/gameover_bg.png");

    loadTexture("StartGameButton", "resources/Button/StartGameButton.png");
    loadTexture("InstructionsButton", "resources/Button/InstructionsButton.png");
    loadTexture("ExitButton", "resources/Button/ExitButton.png");
    loadTexture("FriendButton", "resources/Button/FriendButton.png");
    loadTexture("AiButton", "resources/Button/AiButton.png");
    loadTexture("BackButton", "resources/Button/BackButton.png");
    loadTexture("GenericButton", "resources/Button/Button.png");
    loadTexture("VolumeUpButton", "resources/Button/VolumeUpButton.png");
    loadTexture("VolumeMuteButton", "resources/Button/VolumeMuteButton.png");
    loadTexture("GoToMiniMenuButton", "resources/Button/GoToMiniMenuButton.png");

    loadTexture("heart100", "resources/Heart/Heart100.png");
    loadTexture("game_bg", "resources/Background/Background1.png");
    loadTexture("BonePile", "resources/StaticObject/BonePile.png");

    loadTexture("muffintop", "resources/Monster/Muffintop/Muffintop_R.png");
    loadTexture("muffintop_card", "resources/Monster/Muffintop/Muffintop_card_R.png");

    loadTexture("blue", "resources/Monster/Blue/Blue_R.png");
    loadTexture("blue_card", "resources/Monster/Blue/Blue_card_R.png");

    loadTexture("barzilla", "resources/Monster/Barzilla/Barzilla_R.png");
    loadTexture("barzilla_card", "resources/Monster/Barzilla/Barzilla_card_R.png");

    loadTexture("henrietta", "resources/Monster/Henrietta/Henrietta_R.png");
    loadTexture("henrietta_card", "resources/Monster/Henrietta/Henrietta_card_R.png");

    loadTexture("mozzy", "resources/Monster/Mozzy/Mozzy_R.png");
    loadTexture("mozzy_card", "resources/Monster/Mozzy/Mozzy_card_R.png");

    loadTexture("acid_splash", "resources/Attacks/AcidSplash.png");
    loadTexture("fire_blast", "resources/Attacks/FireBlast.png");
    loadTexture("muffin_shot", "resources/Attacks/MuffinShot.png");
    loadTexture("wind_blast", "resources/Attacks/WindBlast.png");
    loadTexture("flame_web", "resources/Attacks/FlameWeb.png");

    loadTexture("heal_effect", "resources/SpecialAttack/HealEffect.png");
}

void AssetsManager::loadTexture(const std::string& name, const std::string& filePath)
{
    if (m_textures.find(name) != m_textures.end())
        return;

    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(filePath))
        throw std::runtime_error("Failed to load texture: " + filePath);

    m_textures[name] = std::move(texture);
}

const sf::Texture& AssetsManager::getTexture(const std::string& name) const
{
    return *m_textures.at(name);
}

void AssetsManager::loadFont(const std::string& name, const std::string& filePath)
{
    if (m_fonts.find(name) != m_fonts.end())
        return;

    auto font = std::make_unique<sf::Font>();
    if (!font->openFromFile(filePath))
        throw std::runtime_error("Failed to load font: " + filePath);

    m_fonts[name] = std::move(font);
}

const sf::Font& AssetsManager::getFont(const std::string& name) const
{
    return *m_fonts.at(name);
}

void AssetsManager::loadMusic(const std::string& name, const std::string& filePath)
{
    if (m_music.find(name) != m_music.end())
        return;

    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(filePath))
        throw std::runtime_error("Failed to load music: " + filePath);

    m_music[name] = std::move(music);
}

sf::Music& AssetsManager::getMusic(const std::string& name) const
{
    return *m_music.at(name);
}
