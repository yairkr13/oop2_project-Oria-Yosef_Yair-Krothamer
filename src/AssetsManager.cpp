#include "AssetsManager.h"
#include <stdexcept>

AssetsManager& AssetsManager::getInstance()
{
    static AssetsManager instance;
    return instance;
}

// Loads only the loading screen's own two images - small enough to block on
// without delaying the first frame.
void AssetsManager::loadBootAssets()
{
    load<sf::Texture>("await_bg", "resources/Menu/AwaitScreen.png");
    load<sf::Texture>("spinner", "resources/Menu/Spinner.png");
}

// Queues every other asset the game needs instead of loading it immediately -
// loadNext() drains this one entry per frame (see LoadingState).
void AssetsManager::queueRemainingAssets()
{
    using Kind = PendingAsset::Kind;

    m_pendingAssets = {
    { Kind::Font, "arial", "resources/Fonts/arial.ttf" },
    { Kind::Font, "Lilita", "resources/Fonts/LilitaOne.ttf" },

    { Kind::Music, "menu_music", "resources/Music/BackGround/BackGroundMusic.mp3" },
    { Kind::Music, "gameplay_music", "resources/Music/BackGround/InGameMusic.mp3" },

    { Kind::Texture, "menu_bg", "resources/Menu/Menu.png" },
    { Kind::Texture, "instructions_bg_1", "resources/Menu/Instructions1.png" },
    { Kind::Texture, "instructions_bg_2", "resources/Menu/Instructions2.png" },
    { Kind::Texture, "instructions_bg_3", "resources/Menu/Instructions3.png" },
    { Kind::Texture, "mini_menu_bg", "resources/Menu/MiniMenu.png" },
    { Kind::Texture, "gameover_bg", "resources/Menu/gameover_bg.png" },

    { Kind::Texture, "StartGameButton", "resources/Button/StartGameButton.png" },
    { Kind::Texture, "InstructionsButton", "resources/Button/InstructionsButton.png" },
    { Kind::Texture, "ExitButton", "resources/Button/ExitButton.png" },
    { Kind::Texture, "FriendButton", "resources/Button/FriendButton.png" },
    { Kind::Texture, "AiButton", "resources/Button/AiButton.png" },
    { Kind::Texture, "RemoteButton", "resources/Button/remoteButton.png" },
    { Kind::Texture, "BackButton", "resources/Button/BackButton.png" },
    { Kind::Texture, "GenericButton", "resources/Button/Button.png" },
    { Kind::Texture, "VolumeUpButton", "resources/Button/VolumeUpButton.png" },
    { Kind::Texture, "VolumeMuteButton", "resources/Button/VolumeMuteButton.png" },
    { Kind::Texture, "SoundMuteButton", "resources/Button/sound_off_button2.png" },
    { Kind::Texture, "SoundUpButton", "resources/Button/sound_on_button.png" },
    { Kind::Texture, "GoToMiniMenuButton", "resources/Button/GoToMiniMenuButton.png" },
    { Kind::Texture, "NextMenuButton", "resources/Button/NextMenuButton.png" },
    { Kind::Texture, "BackMenuButton", "resources/Button/BackMenuButton.png" },

    { Kind::Texture, "heart_blue", "resources/Heart/BlueHeart.png" },
    { Kind::Texture, "heart_orange", "resources/Heart/OrangeHeart.png" },
    // Both loaded up front - GameplayState picks one at random per game.
    { Kind::Texture, "game_bg_1", "resources/Background/Background1.png" },
    { Kind::Texture, "game_bg_2", "resources/Background/BackGround2.png" },

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

    { Kind::SoundBuffer, "attack_hit", "resources/Sounds/attack_hit.mp3" },
    { Kind::SoundBuffer, "hover_on_button", "resources/Sounds/hover_button.mp3" },
    { Kind::SoundBuffer, "button_click", "resources/Sounds/click-button.mp3" },
    { Kind::SoundBuffer, "dead_sound", "resources/Sounds/death-bong.mp3" },
    { Kind::SoundBuffer, "summon_sound", "resources/Sounds/summon.mp3" },
    { Kind::SoundBuffer, "activate_card", "resources/Sounds/card_sound.mp3" },
    { Kind::SoundBuffer, "parry_attack", "resources/Sounds/sword-deflection.mp3" },
    };

    m_nextPendingIndex = 0;
}

// Loads exactly one queued asset. Returns false once the queue is drained -
// the signal for the caller (LoadingState) to stop calling and move on.
bool AssetsManager::loadNext()
{
    if (m_nextPendingIndex >= m_pendingAssets.size())
        return false;

    const PendingAsset& asset = m_pendingAssets[m_nextPendingIndex];

    switch (asset.kind)
    {
    case PendingAsset::Kind::Texture:     load<sf::Texture>(asset.name, asset.filePath); break;
    case PendingAsset::Kind::Font:        load<sf::Font>(asset.name, asset.filePath);    break;
    case PendingAsset::Kind::Music:       load<sf::Music>(asset.name, asset.filePath);   break;
    case PendingAsset::Kind::SoundBuffer: load<sf::SoundBuffer>(asset.name, asset.filePath); break;
    default:
        throw std::logic_error("AssetsManager::loadNext: unhandled PendingAsset::Kind for queued asset '"
            + asset.name + "'");
    }

    // Only reached if the load above didn't throw.
    ++m_nextPendingIndex;

    return true;
}

const sf::Texture& AssetsManager::getTexture(const std::string& name) const
{
    return get<sf::Texture>(name);
}

const sf::Font& AssetsManager::getFont(const std::string& name) const
{
    return get<sf::Font>(name);
}

// Non-const on purpose, unlike the other getters: callers need to control
// playback (play/pause/stop), not just read the asset.
sf::Music& AssetsManager::getMusic(const std::string& name) const
{
    return const_cast<sf::Music&>(get<sf::Music>(name));
}

const sf::SoundBuffer& AssetsManager::getSoundBuffer(const std::string& name) const
{
    return get<sf::SoundBuffer>(name);
}
