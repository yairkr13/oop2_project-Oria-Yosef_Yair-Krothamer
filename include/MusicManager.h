#pragma once
#include "MusicTrack.h"
#include <string>

// Owns which looping background track (if any) is playing and the shared
// mute flag - the single source of truth for playback and every toggle
// button's icon. States just declare a track via State::desiredMusicTrack();
// Controller drives playTrack() each frame.
class MusicManager
{
public:
    MusicManager(const MusicManager&) = delete;
    MusicManager& operator=(const MusicManager&) = delete;

    static MusicManager& getInstance();

    void playTrack(MusicTrack track);

    void toggleMute();
    bool isMuted() const;

private:
    MusicManager() = default;

    static const std::string& assetNameFor(MusicTrack track);

    MusicTrack m_currentTrack = MusicTrack::None;
    bool m_muted = false;
};
