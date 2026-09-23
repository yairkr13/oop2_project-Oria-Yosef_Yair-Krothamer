#include "AssetsManager.h"
#include <stdexcept>

void AssetsManager::loadBootAssets()
{
   /* loadTexture("await_bg", "resources/Menu/AwaitScreen.png");
    loadTexture("spinner", "resources/Menu/Spinner.png");*/
    load<sf::Texture>("await_bg", "resources/Menu/AwaitScreen.png");
    load<sf::Texture>("spinner", "resources/Menu/Spinner.png");
}

void AssetsManager::queueRemainingAssets() //��� ��� ��? ��� ���� ���� ���� �� ������� �������� ���� ���??????
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
    // Both loaded up front (GameplayState picks one at random per new game
    // - see GameplayState::randomGameBackgroundKey) rather than just the
    // one "game_bg" this used to be - loadNext() drains the whole queue
    // before MenuState is ever reachable, so both are already available
    // by the time any GameplayState gets constructed.
    { Kind::Texture, "game_bg_1", "resources/Background/Background1.png" },
    { Kind::Texture, "game_bg_2", "resources/Background/BackGround2.png" },
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

   /*  Advance only once the asset above actually loaded - if loadX threw, the
     index still points at the failed entry (irrelevant while we unwind to
     main(), but keeps this function's own state honest).*/
    ++m_nextPendingIndex;

    return true;
}

//bool AssetsManager::loadNext()
//{
//    if (m_nextPendingIndex >= m_pendingAssets.size())
//        return false;
//
//    const PendingAsset& asset = m_pendingAssets[m_nextPendingIndex];
//    switch (asset.kind)
//    {//��� ���� �� �� ������ ����????���� ����� ������� �������
//    case PendingAsset::Kind::Texture: loadTexture(asset.name, asset.filePath); break;
//    case PendingAsset::Kind::Font:    loadFont(asset.name, asset.filePath);    break;
//    case PendingAsset::Kind::Music:   loadMusic(asset.name, asset.filePath);   break;
//    case PendingAsset::Kind::SoundBuffer: loadSoundBuffer(asset.name, asset.filePath); break;
//    default:
//        throw std::logic_error("AssetsManager::loadNext: unhandled PendingAsset::Kind for queued asset '"
//            + asset.name + "'");
//    }
//
//    // Advance only once the asset above actually loaded - if loadX threw, the
//    // index still points at the failed entry (irrelevant while we unwind to
//    // main(), but keeps this function's own state honest).
//    ++m_nextPendingIndex;
//    return true;
//}

//
//template <typename T>
//void AssetsManager::loadAsset(std::unordered_map<std::string, std::unique_ptr<T>>& map,
//    const std::string& name, const std::string& filePath,
//    bool (T::* openMethod)(const std::string&), const char* typeLabel)
//{
//    if (map.find(name) != map.end())
//        return;
//
//    auto asset = std::make_unique<T>();
//    if (!((*asset).*openMethod)(filePath))
//        throw std::runtime_error(std::string("AssetsManager: failed to load ") + typeLabel + " '" + name
//            + "' from \"" + filePath + "\" (file missing, unreadable, or not a valid " + typeLabel + ")");
//
//    map[name] = std::move(asset);
//}
//
//template <typename T>
//const T& AssetsManager::getAsset(const std::unordered_map<std::string, std::unique_ptr<T>>& map,
//    const std::string& name, const char* typeLabel) const
//{
//    auto it = map.find(name);
//    if (it == map.end())
//        throw std::out_of_range(std::string("AssetsManager: no ") + typeLabel + " registered under '" + name
//            + "' (asset was never loaded - check the key spelling and queueRemainingAssets())");
//
//    return *it->second;
//}
//
//
//

//void AssetsManager::loadTexture(const std::string& name, const std::string& filePath)
//{
//    loadAsset(m_textures, name, filePath, &sf::Texture::loadFromFile, "texture");
//}
//
//const sf::Texture& AssetsManager::getTexture(const std::string& name) const
//{
//    return getAsset(m_textures, name, "texture");
//}
//
//void AssetsManager::loadFont(const std::string& name, const std::string& filePath)
//{
//    loadAsset(m_fonts, name, filePath, &sf::Font::openFromFile, "font");
//}
//
//const sf::Font& AssetsManager::getFont(const std::string& name) const
//{
//    return getAsset(m_fonts, name, "font");
//}
//
//void AssetsManager::loadMusic(const std::string& name, const std::string& filePath)
//{
//    loadAsset(m_music, name, filePath, &sf::Music::openFromFile, "music track");
//}
//
//sf::Music& AssetsManager::getMusic(const std::string& name) const
//{
//    // ���� getAsset<T> ������ ����� ��� ���������/������ - �-const_cast ���
//    // ����� ������: unique_ptr<T>::operator* ���� ����� T& ��-���� (�-constness
//    // �� "���� ����"), �� ��� ���� ����� ������ ����� �� ������ �-lookup ��
//    // ��� ���� ��� ����� ���� - �� ������ �� �-const ����� ��� �����.
//    return const_cast<sf::Music&>(getAsset(m_music, name, "music track"));
//}
//
//void AssetsManager::loadSoundBuffer(const std::string& name, const std::string& filePath)
//{
//    loadAsset(m_soundBuffers, name, filePath, &sf::SoundBuffer::loadFromFile, "sound buffer");
//}
//
//const sf::SoundBuffer& AssetsManager::getSoundBuffer(const std::string& name) const
//{
//    return getAsset(m_soundBuffers, name, "sound buffer");
//}






//void AssetsManager::loadTexture(const std::string& name, const std::string& filePath)
//{
//    if (m_textures.find(name) != m_textures.end())
//        return;
//
//    auto texture = std::make_unique<sf::Texture>();
//    if (!texture->loadFromFile(filePath))
//        throw std::runtime_error("AssetsManager: failed to load texture '" + name
//            + "' from \"" + filePath + "\" (file missing, unreadable, or not a valid image)");
//
//    m_textures[name] = std::move(texture);
//}
//
//const sf::Texture& AssetsManager::getTexture(const std::string& name) const
//{
//    auto it = m_textures.find(name);
//    if (it == m_textures.end())
//        throw std::out_of_range("AssetsManager: no texture registered under '" + name
//            + "' (asset was never loaded - check the key spelling and queueRemainingAssets())");
//
//    return *it->second;
//}
//
//void AssetsManager::loadFont(const std::string& name, const std::string& filePath)
//{
//    if (m_fonts.find(name) != m_fonts.end())
//        return;
//
//    auto font = std::make_unique<sf::Font>();
//    if (!font->openFromFile(filePath))
//        throw std::runtime_error("AssetsManager: failed to load font '" + name
//            + "' from \"" + filePath + "\" (file missing, unreadable, or not a valid font)");
//
//    m_fonts[name] = std::move(font);
//}
//
//const sf::Font& AssetsManager::getFont(const std::string& name) const
//{
//    auto it = m_fonts.find(name);
//    if (it == m_fonts.end())
//        throw std::out_of_range("AssetsManager: no font registered under '" + name
//            + "' (asset was never loaded - check the key spelling and queueRemainingAssets())");
//
//    return *it->second;
//}
//
//void AssetsManager::loadMusic(const std::string& name, const std::string& filePath)
//{
//    if (m_music.find(name) != m_music.end())
//        return;
//
//    auto music = std::make_unique<sf::Music>();
//    if (!music->openFromFile(filePath))
//        throw std::runtime_error("AssetsManager: failed to load music '" + name
//            + "' from \"" + filePath + "\" (file missing, unreadable, or not a valid audio file)");
//
//    m_music[name] = std::move(music);
//}
//
//sf::Music& AssetsManager::getMusic(const std::string& name) const
//{
//    auto it = m_music.find(name);
//    if (it == m_music.end())
//        throw std::out_of_range("AssetsManager: no music registered under '" + name
//            + "' (asset was never loaded - check the key spelling and queueRemainingAssets())");
//
//    return *it->second;
//}
