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

    // Which background track this screen wants playing while it's on top
    // of the stack - MusicTrack::None by default, so states that don't
    // care about music (MiniMenuState, GameOverState, ...) don't need to
    // know anything about it. Controller reads this every frame and asks
    // MusicManager to play it.
    virtual MusicTrack desiredMusicTrack() const { return MusicTrack::None; }

    // Hands ownership of the next state to the caller (Controller).
    // Returns nullptr if the finished state has no successor (e.g. "quit"
    // or "just pop back to whatever's beneath me").
    std::unique_ptr<State> getNextState();

    bool hasStateToPush() const { return m_stateToPush != nullptr; }

    // Hands ownership of the state that should be pushed on top of this one.
    std::unique_ptr<State> getStateToPush();

protected:
    // Finishes this state, popping it off the Controller's state stack.
    // Pass a state to replace it with one for a forward transition (e.g.
    // Start Game). Pass nothing (the default) to just pop, which resumes
    // whatever state is beneath it - e.g. a "Back" button - or closes the
    // window if the stack becomes empty.
    void transitionTo(std::unique_ptr<State> next = nullptr);

    // Suspends this state - it stays alive, paused, beneath `next` on the
    // stack - and shows `next` on top of it. This state resumes
    // automatically once `next` (or whatever it in turn pushes) eventually
    // calls transitionTo() with no argument, popping back down to it.
    void pushState(std::unique_ptr<State> next);

    // True once this state has requested any transition, finish or push.
    // Handy as an early-return guard in handleEvent() so a state stops
    // reacting to further input once it's on its way out.
    bool isTransitioning() const;

private:
    bool m_isFinished = false;
    std::unique_ptr<State> m_nextState;
    std::unique_ptr<State> m_stateToPush;
};
