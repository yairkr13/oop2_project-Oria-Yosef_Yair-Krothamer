#include "Controller.h"
#include "State/LoadingState.h"
#include "Constants.h"
#include "AssetsManager.h"
#include "MusicManager.h"
#include <stdexcept>
#include <string>

Controller::Controller()
{
	
    // Runtime check instead of static_assert - main() catches this and
    // prints a clean message rather than a wall of compiler errors.
    if (Config::WINDOW_WIDTH < Config::MIN_WINDOW_WIDTH || Config::WINDOW_WIDTH > Config::MAX_WINDOW_WIDTH ||
        Config::WINDOW_HEIGHT < Config::MIN_WINDOW_HEIGHT || Config::WINDOW_HEIGHT > Config::MAX_WINDOW_HEIGHT)
    {
        //check if the size of the window is within the supported range, if not throw an exception
        throw std::out_of_range(
            "Config::WINDOW_WIDTH/HEIGHT (" + std::to_string(Config::WINDOW_WIDTH) + "x" +
            std::to_string(Config::WINDOW_HEIGHT) + ") is outside the supported range (" +
            std::to_string(Config::MIN_WINDOW_WIDTH) + "-" + std::to_string(Config::MAX_WINDOW_WIDTH) + " x " +
            std::to_string(Config::MIN_WINDOW_HEIGHT) + "-" + std::to_string(Config::MAX_WINDOW_HEIGHT) + ")");
    }

    // No sf::Style::Resize - fixed size, so positions computed from
    // Config::WINDOW_WIDTH/HEIGHT stay valid with no resize handling elsewhere.
    m_window.create(sf::VideoMode({ Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT }), "Phobies",
        sf::Style::Titlebar | sf::Style::Close);
    m_window.setFramerateLimit(60);

    // Only the loading screen's own assets load synchronously here; the rest
    // is queued and drained incrementally by LoadingState.
    AssetsManager::getInstance().loadBootAssets();

    m_states.push_back(std::make_unique<LoadingState>(m_window));
}

void Controller::run() 
{
    while (m_window.isOpen())
    {
        sf::Time deltaTime = m_clock.restart();
        
        // Forward every pending event to the current state; a Closed event closes the window.
        while (const auto event = m_window.pollEvent())
        {
            m_states.back()->handleEvent(*event);
            if (event->is<sf::Event::Closed>())
                m_window.close();
        }

		// manage the current state and check if it wants to push a new state or pop itself.
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
