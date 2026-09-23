#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

// Plays short one-off sound effects by name, looking up buffers via
// AssetsManager. Kept separate from AssetsManager: that class only loads and
// owns assets, this one only manages playback.
class SoundPlayer
{
public:
    SoundPlayer(const SoundPlayer&) = delete;
    SoundPlayer& operator=(const SoundPlayer&) = delete;

    static SoundPlayer& getInstance();

    void play(const std::string& name);

    void toggleMute();
    bool isMuted() const;

private:
    SoundPlayer() = default;

    // sf::Sound only plays while the object stays alive - these keep each
    // play() call's sound alive until it finishes.
    std::vector<sf::Sound> m_activeSounds;
    bool m_muted = false;
};