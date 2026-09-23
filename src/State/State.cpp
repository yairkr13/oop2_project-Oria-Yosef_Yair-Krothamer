#include "State/State.h"

// Ownership of the replacement state queued by transitionTo(), if any.
std::unique_ptr<State> State::getNextState()
{
    return std::move(m_nextState);
}

std::unique_ptr<State> State::getStateToPush()
{
    return std::move(m_stateToPush);
}

// Finishes this state, popping it off the stack. Pass a replacement for a
// forward transition, or nothing to just pop back to whatever's beneath it.
void State::transitionTo(std::unique_ptr<State> next)
{
    m_isFinished = true;
    m_nextState = std::move(next);
}

// Suspends this state beneath `next`; it resumes once `next` eventually
// calls transitionTo() with no argument.
void State::pushState(std::unique_ptr<State> next)
{
    m_stateToPush = std::move(next);
}
