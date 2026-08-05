#include "Controller.h"
#include "State/MenuState.h"
#include "Constants.h"
#include "AssetsManager.h"

Controller::Controller()
{
    m_window.create(sf::VideoMode({ Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT }), "Phobies");
    m_window.setFramerateLimit(60);

    AssetsManager::getInstance().loadAllAssets();

    m_states.push_back(std::make_unique<MenuState>(m_window));
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

        m_states.back()->update(deltaTime);

        m_window.clear(sf::Color::Black);
        for (auto& state : m_states)
            state->draw(m_window);
        m_window.display();
    }
}
