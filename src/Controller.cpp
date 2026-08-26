#include "Controller.h"
#include "State/LoadingState.h"
#include "Constants.h"
#include "AssetsManager.h"
#include "MusicManager.h"

Controller::Controller()
{
    m_window.create(sf::VideoMode({ Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT }), "Phobies");
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
