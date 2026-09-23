#include "State/State.h"

bool State::isFinished() const
{
    return m_isFinished;
}

// Which background track this screen wants playing while it's on top
// of the stack - MusicTrack::None by default, so states that don't
// care about music (MiniMenuState, GameOverState, ...) don't need to
// know anything about it. Controller reads this every frame and asks
// MusicManager to play it.
MusicTrack State::desiredMusicTrack() const
{
    return MusicTrack::None;
}

std::unique_ptr<State> State::getNextState()
{
    return std::move(m_nextState);
}

bool State::hasStateToPush() const
{
    return m_stateToPush != nullptr;
}

std::unique_ptr<State> State::getStateToPush()
{
    return std::move(m_stateToPush);
}

void State::transitionTo(std::unique_ptr<State> next)
{
    m_isFinished = true;
    m_nextState = std::move(next);
}

void State::pushState(std::unique_ptr<State> next)
{
    m_stateToPush = std::move(next);
}
