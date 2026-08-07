#pragma once

// Which looping background track (if any) a State wants playing while it's
// on top of the stack. See State::desiredMusicTrack() and MusicManager.
enum class MusicTrack
{
    None,
    Menu,
    Gameplay
};
