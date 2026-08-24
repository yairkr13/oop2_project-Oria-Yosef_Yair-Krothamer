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

    loadTexture("muffintop", "resources/Monster/Muffintop/Muffintop.png");
    loadTexture("muffintop_card", "resources/Monster/Muffintop/Muffintop_Card.png");
    loadTexture("muffintop_walk", "resources/Monster/Muffintop/Muffintop_Walk.png");
    loadTexture("muffintop_attack", "resources/Monster/Muffintop/Muffintop_Attack.png");
    loadTexture("muffintop_idle", "resources/Monster/Muffintop/Muffintop_Idle.png");
    loadTexture("muffintop_die", "resources/Monster/Muffintop/Muffintop_Die.png");

    loadTexture("blue", "resources/Monster/Blue/Blue.png");
    loadTexture("blue_card", "resources/Monster/Blue/Blue_Card.png");
    loadTexture("blue_fly", "resources/Monster/Blue/Blue_Fly.png"); // flying monster's movement animation
    loadTexture("blue_idle", "resources/Monster/Blue/Blue_Idle.png");
    loadTexture("blue_attack", "resources/Monster/Blue/Blue_Attack.png");
    loadTexture("blue_die", "resources/Monster/Blue/Blue_Die.png");

    loadTexture("barzilla", "resources/Monster/Barzilla/Barzilla.png");
    loadTexture("barzilla_card", "resources/Monster/Barzilla/Barzilla_Card.png");
    loadTexture("barzilla_walk", "resources/Monster/Barzilla/Barzilla_Walk.png");
    loadTexture("barzilla_idle", "resources/Monster/Barzilla/Barzilla_Idle.png");
    loadTexture("barzilla_attack", "resources/Monster/Barzilla/Barzilla_Attack.png");
    loadTexture("barzilla_die", "resources/Monster/Barzilla/Barzilla_Die.png");

    loadTexture("henrietta", "resources/Monster/Henrietta/Henrietta.png");
    loadTexture("henrietta_card", "resources/Monster/Henrietta/Henrietta_Card.png");
    loadTexture("henrietta_walk", "resources/Monster/Henrietta/Henrietta_Walk.png");
    loadTexture("henrietta_idle", "resources/Monster/Henrietta/Henrietta_Idle.png");
    loadTexture("henrietta_attack", "resources/Monster/Henrietta/Henrietta_Attack.png");
    loadTexture("henrietta_die", "resources/Monster/Henrietta/Henrietta_Die.png");

    loadTexture("mozzy", "resources/Monster/Mozzy/Mozzy.png");
    loadTexture("mozzy_card", "resources/Monster/Mozzy/Mozzy_Card.png");
    loadTexture("mozzy_fly", "resources/Monster/Mozzy/Mozzy_Fly.png"); // flying monster's movement animation
    loadTexture("mozzy_idle", "resources/Monster/Mozzy/Mozzy_Idle.png");
    loadTexture("mozzy_attack", "resources/Monster/Mozzy/Mozzy_Attack.png");
    loadTexture("mozzy_die", "resources/Monster/Mozzy/Mozzy_Die.png");

    loadTexture("acid_splash", "resources/Attacks/AcidSplash.png");
    loadTexture("fire_blast", "resources/Attacks/FireBlast.png");
    loadTexture("muffin_shot", "resources/Attacks/MuffinShot.png");
    loadTexture("wind_blast", "resources/Attacks/WindBlast.png");
    loadTexture("flame_web", "resources/Attacks/FlameWeb.png");

    loadTexture("heal_effect", "resources/SpecialAttack/HealEffect.png");
    loadTexture("freeze_effect", "resources/SpecialAttack/FreezeEffect.png");
    loadTexture("wind_effect", "resources/SpecialAttack/WindEffect.png");
    loadTexture("shield_effect", "resources/SpecialAttack/ShieldEffect.png");
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
