#include "State/State.h"

std::unique_ptr<State> State::getNextState()
{
    return std::move(m_nextState);
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

bool State::isTransitioning() const
{
    return m_isFinished || m_stateToPush != nullptr;
}
