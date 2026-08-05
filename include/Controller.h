#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "State/State.h"

// Owns the window, the clock and the main loop. Keeps a stack of States:
// only the top one runs, and it drives what's on top next by finishing
// (optionally with a replacement) or pushing a new state over itself.
class Controller
{
public:
    Controller();
    void run();

private:
    sf::RenderWindow m_window;
    sf::Clock m_clock;
    std::vector<std::unique_ptr<State>> m_states;
};
