#include "Controller.h"
#include "State/LoadingState.h"
#include "Constants.h"
#include "AssetsManager.h"
#include "MusicManager.h"
#include <stdexcept>
#include <string>

Controller::Controller()
{
    // Checked here, before the window is actually created, rather than a
    // static_assert in Constants.h - main() already wraps Controller's
    // construction in a try/catch (see main.cpp) that prints e.what()
    // cleanly, a clearer failure than the wall of repeated compiler errors
    // a static_assert in a header included almost everywhere produces.
    if (Config::WINDOW_WIDTH < Config::MIN_WINDOW_WIDTH || Config::WINDOW_WIDTH > Config::MAX_WINDOW_WIDTH ||
        Config::WINDOW_HEIGHT < Config::MIN_WINDOW_HEIGHT || Config::WINDOW_HEIGHT > Config::MAX_WINDOW_HEIGHT)
    {
        throw std::out_of_range(
            "Config::WINDOW_WIDTH/HEIGHT (" + std::to_string(Config::WINDOW_WIDTH) + "x" +
            std::to_string(Config::WINDOW_HEIGHT) + ") is outside the supported range (" +
            std::to_string(Config::MIN_WINDOW_WIDTH) + "-" + std::to_string(Config::MAX_WINDOW_WIDTH) + " x " +
            std::to_string(Config::MIN_WINDOW_HEIGHT) + "-" + std::to_string(Config::MAX_WINDOW_HEIGHT) + ")");
    }

    // No sf::Style::Resize - the window is a fixed size for its whole
    // lifetime (no maximize button, no draggable edges), so every position
    // computed from Config::WINDOW_WIDTH/HEIGHT stays valid without any
    // resize handling anywhere else in the game.
    m_window.create(sf::VideoMode({ Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT }), "Phobies",
        sf::Style::Titlebar | sf::Style::Close);
    m_window.setFramerateLimit(60);

    // Only the loading screen's own two assets are loaded synchronously
    // here - cheap enough (two images) not to delay the first frame.
    // Everything else is queued and drained incrementally by LoadingState
    // itself (see AssetsManager::queueRemainingAssets/loadNext), so run()'s
    // loop below can actually start presenting frames - AwaitScreen plus a
    // spinning Spinner - well before the rest of the game's assets (menu/
    // button textures, fonts, music, every monster's sprite sheets, ...)
    // are ready, instead of blocking on one big load before the window
    // ever shows anything.
    AssetsManager::getInstance().loadBootAssets();

    m_states.push_back(std::make_unique<LoadingState>(m_window));
}

void Controller::run() 
{
    while (m_window.isOpen())
    {
        sf::Time deltaTime = m_clock.restart();
        

        while (const auto event = m_window.pollEvent())
        {
            m_states.back()->handleEvent(*event);
            if (event->is<sf::Event::Closed>())
                m_window.close();
        }

        State& current = *m_states.back();
        if (current.hasStateToPush())
        {
            m_states.push_back(current.getStateToPush());
        }
        else if (current.isFinished())
        {
            auto next = current.getNextState();
            m_states.pop_back();
            if (next)
                m_states.push_back(std::move(next));
        }

        if (m_states.empty())
        {
            m_window.close();
            break;
        }

        MusicManager::getInstance().playTrack(m_states.back()->desiredMusicTrack());

        m_states.back()->update(deltaTime);

        m_window.clear(sf::Color::Black);
        for (auto& state : m_states)
            state->draw(m_window);
        m_window.display();
    }
}
