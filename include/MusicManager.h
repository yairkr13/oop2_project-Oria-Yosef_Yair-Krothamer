#pragma once
#include "MusicTrack.h"
#include <string>

// Owns which of the looping background tracks (if any) is currently
// playing, and the single mute flag that applies to whichever one is
// active - the shared source of truth for both actual playback and every
// MusicToggleButton's displayed icon. States never touch sf::Music
// directly: they just declare which track they want via
// State::desiredMusicTrack(); Controller reads that every frame and calls
// playTrack() here, which is a no-op if it's already the active track.
class MusicManager
{
public:
    MusicManager(const MusicManager&) = delete;
    MusicManager& operator=(const MusicManager&) = delete;

    static MusicManager& getInstance()
    {
        static MusicManager instance;
        return instance;
    }

    // Plays `track` looping if it isn't already the active one - stopping
    // whatever else was playing first. MusicTrack::None just stops.
    void playTrack(MusicTrack track);

    void toggleMute();
    bool isMuted() const { return m_muted; }

private:
    MusicManager() = default;

    static const std::string& assetNameFor(MusicTrack track);

    MusicTrack m_currentTrack = MusicTrack::None;
    bool m_muted = false;
};
