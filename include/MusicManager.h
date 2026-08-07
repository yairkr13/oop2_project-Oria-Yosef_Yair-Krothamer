#pragma once

// Owns playback of the menu background music - the single place that
// decides whether it's currently allowed to play, and holds the mute
// state that the mini-menu's volume button toggles. Callers never touch
// the underlying sf::Music (owned by AssetsManager) directly.
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

    // Starts the menu music looping if it isn't already playing. Safe to
    // call every frame - a no-op once it's underway.
    void playMenuMusic();

    // Stops the menu music if it's playing. Safe to call every frame.
    void stopMenuMusic();

    void toggleMute();
    bool isMuted() const { return m_muted; }

private:
    MusicManager() = default;

    bool m_muted = false;
};
