#include "AssetsManager.h"
#include <stdexcept>

void AssetsManager::loadBootAssets()
{
    loadTexture("await_bg", "resources/Menu/AwaitScreen.png");
    loadTexture("spinner", "resources/Menu/Spinner.png");
}

void AssetsManager::queueRemainingAssets()
{
    using Kind = PendingAsset::Kind;

    // Same assets loadAllAssets() used to load immediately, in the same
    // order - now queued instead, so loadNext() can drain them one at a
    // time across frames (see the class comment in AssetsManager.h).
    m_pendingAssets = {
    { Kind::Font, "arial", "resources/Fonts/arial.ttf" },
    { Kind::Font, "Lilita", "resources/Fonts/LilitaOne.ttf" },

    { Kind::Music, "menu_music", "resources/Music/BackGround/BackGroundMusic.mp3" },
    { Kind::Music, "gameplay_music", "resources/Music/BackGround/InGameMusic.mp3" },

    { Kind::Texture, "menu_bg", "resources/Menu/Menu.png" },
    { Kind::Texture, "instructions_bg", "resources/Menu/Instructions.png" },
    { Kind::Texture, "mini_menu_bg", "resources/Menu/MiniMenu.png" },
    { Kind::Texture, "gameover_bg", "resources/Menu/gameover_bg.png" },

    { Kind::Texture, "StartGameButton", "resources/Button/StartGameButton.png" },
    { Kind::Texture, "InstructionsButton", "resources/Button/InstructionsButton.png" },
    { Kind::Texture, "ExitButton", "resources/Button/ExitButton.png" },
    { Kind::Texture, "FriendButton", "resources/Button/FriendButton.png" },
    { Kind::Texture, "AiButton", "resources/Button/AiButton.png" },
    { Kind::Texture, "BackButton", "resources/Button/BackButton.png" },
    { Kind::Texture, "GenericButton", "resources/Button/Button.png" },
    { Kind::Texture, "VolumeUpButton", "resources/Button/VolumeUpButton.png" },
    { Kind::Texture, "VolumeMuteButton", "resources/Button/VolumeMuteButton.png" },
    { Kind::Texture, "GoToMiniMenuButton", "resources/Button/GoToMiniMenuButton.png" },

    { Kind::Texture, "heart100", "resources/Heart/Heart100.png" },
    { Kind::Texture, "game_bg", "resources/Background/Background1.png" },
    { Kind::Texture, "BonePile", "resources/StaticObject/BonePile.png" },

    { Kind::Texture, "muffintop", "resources/Monster/Muffintop/Muffintop.png" },
    { Kind::Texture, "muffintop_card", "resources/Monster/Muffintop/Muffintop_Card.png" },
    { Kind::Texture, "muffintop_walk", "resources/Monster/Muffintop/Muffintop_Walk.png" },
    { Kind::Texture, "muffintop_attack", "resources/Monster/Muffintop/Muffintop_Attack.png" },
    { Kind::Texture, "muffintop_idle", "resources/Monster/Muffintop/Muffintop_Idle.png" },
    { Kind::Texture, "muffintop_die", "resources/Monster/Muffintop/Muffintop_Die.png" },

    { Kind::Texture, "blue", "resources/Monster/Blue/Blue.png" },
    { Kind::Texture, "blue_card", "resources/Monster/Blue/Blue_Card.png" },
    { Kind::Texture, "blue_fly", "resources/Monster/Blue/Blue_Fly.png" }, // flying monster's movement animation
    { Kind::Texture, "blue_idle", "resources/Monster/Blue/Blue_Idle.png" },
    { Kind::Texture, "blue_attack", "resources/Monster/Blue/Blue_Attack.png" },
    { Kind::Texture, "blue_die", "resources/Monster/Blue/Blue_Die.png" },

    { Kind::Texture, "barzilla", "resources/Monster/Barzilla/Barzilla.png" },
    { Kind::Texture, "barzilla_card", "resources/Monster/Barzilla/Barzilla_Card.png" },
    { Kind::Texture, "barzilla_walk", "resources/Monster/Barzilla/Barzilla_Walk.png" },
    { Kind::Texture, "barzilla_idle", "resources/Monster/Barzilla/Barzilla_Idle.png" },
    { Kind::Texture, "barzilla_attack", "resources/Monster/Barzilla/Barzilla_Attack.png" },
    { Kind::Texture, "barzilla_die", "resources/Monster/Barzilla/Barzilla_Die.png" },

    { Kind::Texture, "henrietta", "resources/Monster/Henrietta/Henrietta.png" },
    { Kind::Texture, "henrietta_card", "resources/Monster/Henrietta/Henrietta_Card.png" },
    { Kind::Texture, "henrietta_walk", "resources/Monster/Henrietta/Henrietta_Walk.png" },
    { Kind::Texture, "henrietta_idle", "resources/Monster/Henrietta/Henrietta_Idle.png" },
    { Kind::Texture, "henrietta_attack", "resources/Monster/Henrietta/Henrietta_Attack.png" },
    { Kind::Texture, "henrietta_die", "resources/Monster/Henrietta/Henrietta_Die.png" },

    { Kind::Texture, "mozzy", "resources/Monster/Mozzy/Mozzy.png" },
    { Kind::Texture, "mozzy_card", "resources/Monster/Mozzy/Mozzy_Card.png" },
    { Kind::Texture, "mozzy_fly", "resources/Monster/Mozzy/Mozzy_Fly.png" }, // flying monster's movement animation
    { Kind::Texture, "mozzy_idle", "resources/Monster/Mozzy/Mozzy_Idle.png" },
    { Kind::Texture, "mozzy_attack", "resources/Monster/Mozzy/Mozzy_Attack.png" },
    { Kind::Texture, "mozzy_die", "resources/Monster/Mozzy/Mozzy_Die.png" },

    { Kind::Texture, "acid_splash", "resources/Attacks/AcidSplash.png" },
    { Kind::Texture, "fire_blast", "resources/Attacks/FireBlast.png" },
    { Kind::Texture, "muffin_shot", "resources/Attacks/MuffinShot.png" },
    { Kind::Texture, "wind_blast", "resources/Attacks/WindBlast.png" },
    { Kind::Texture, "flame_web", "resources/Attacks/FlameWeb.png" },

    { Kind::Texture, "heal_effect", "resources/SpecialAttack/HealEffect.png" },
    { Kind::Texture, "freeze_effect", "resources/SpecialAttack/FreezeEffect.png" },
    { Kind::Texture, "wind_effect", "resources/SpecialAttack/WindEffect.png" },
    { Kind::Texture, "shield_effect", "resources/SpecialAttack/ShieldEffect.png" },
    };

    m_nextPendingIndex = 0;
}

bool AssetsManager::loadNext()
{
    if (m_nextPendingIndex >= m_pendingAssets.size())
        return false;

    const PendingAsset& asset = m_pendingAssets[m_nextPendingIndex++];
    switch (asset.kind)
    {
    case PendingAsset::Kind::Texture: loadTexture(asset.name, asset.filePath); break;
    case PendingAsset::Kind::Font:    loadFont(asset.name, asset.filePath);    break;
    case PendingAsset::Kind::Music:   loadMusic(asset.name, asset.filePath);   break;
    }
    return true;
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
