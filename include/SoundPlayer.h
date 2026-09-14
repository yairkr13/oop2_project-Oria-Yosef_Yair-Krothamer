#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

// Plays short sound effects by name (see AssetsManager::getSoundBuffer for
// where the underlying data comes from) - this class's only responsibility
// is managing the *playback* of sounds, not loading or owning them. Kept
// separate from AssetsManager on purpose: loading/owning assets and
// actually playing one back are two different jobs that happen to both
// involve audio - AssetsManager already has its own single responsibility
// (load once, hand out references), and mixing "orchestrate playback" into
// that would blur it, the same way Board staying out of AI's decisions kept
// Board's own responsibility clean.
//
// sf::Sound doesn't own its audio and only plays while the sf::Sound object
// itself stays alive - unlike sf::Music, there's no single long-lived
// object a caller already holds onto. This class exists so a caller can
// just fire a one-off sound and move on, without managing that lifetime
// itself: play() keeps the sf::Sound instance alive internally for exactly
// as long as it's actually playing.
class SoundPlayer
{
public:
    SoundPlayer(const SoundPlayer&) = delete;
    SoundPlayer& operator=(const SoundPlayer&) = delete;

    static SoundPlayer& getInstance()
    {
        static SoundPlayer instance;
        return instance;
    }

    // Fire-and-forget: looks up `name`'s buffer (via AssetsManager), builds
    // an sf::Sound bound to it, and plays it immediately. Safe to call many
    // times in the same frame (e.g. several attacks resolving at once) -
    // each call gets its own independent sf::Sound, so overlapping calls
    // don't cut each other off the way replaying one shared sf::Sound would.
    void play(const std::string& name);

    void toggleMute() { m_muted = !m_muted; }
    bool isMuted() const { return m_muted; }

private:
    SoundPlayer() = default;

    // Sounds currently playing, started by play() above - this is the only
    // thing keeping them alive (see the class comment). Pruned of finished
    // sounds every time play() is called, so this never grows without
    // bound over a long play session even though nothing calls back in to
    // clean it up on its own.
    std::vector<sf::Sound> m_activeSounds;
    bool m_muted = false;
};