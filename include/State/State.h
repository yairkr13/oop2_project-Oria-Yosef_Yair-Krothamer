#pragma once
#include <SFML/Graphics.hpp>
#include "MusicTrack.h"
#include <memory>

// Abstract base class for every game state (MainMenu, Gameplay, Pause, Victory, ...).
// The Controller keeps a stack of states: only the top one runs (handles
// events, updates, draws). States beneath it are paused, not destroyed.
class State
{
public:
    virtual ~State() = default;

    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual void update(sf::Time deltaTime) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;

    bool isFinished() const { return m_isFinished; }

    // Track this state wants playing while it's on top of the stack; None by default.
    virtual MusicTrack desiredMusicTrack() const { return MusicTrack::None; }

    std::unique_ptr<State> getNextState();

    bool hasStateToPush() const { return m_stateToPush != nullptr; }

    std::unique_ptr<State> getStateToPush();

protected:
    void transitionTo(std::unique_ptr<State> next = nullptr);

    void pushState(std::unique_ptr<State> next);

private:
    bool m_isFinished = false;
    std::unique_ptr<State> m_nextState;
    std::unique_ptr<State> m_stateToPush;
};
