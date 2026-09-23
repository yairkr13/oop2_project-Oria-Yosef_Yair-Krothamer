#include "State/NetworkLobbyState.h"
#include "State/GameplayState.h"
#include "AssetsManager.h"
#include "SpriteUtils.h"
#include "Constants.h"

namespace
{
    // Arbitrary port; the game only ever needs the one LAN connection.
    constexpr unsigned short PORT = 54321;

    constexpr unsigned int MENU_BUTTON_WIDTH = 260;
    constexpr float MENU_GAP = 18.f;

    // Offsets from the window's own vertical center, Config-based instead of live window size.
    constexpr float STATUS_TEXT_OFFSET_FROM_CENTER = -220.f;
    constexpr float ADDRESS_TEXT_OFFSET_FROM_CENTER = -130.f;
    constexpr float MENU_TOP_OFFSET_FROM_CENTER = -20.f; // where the button stack starts

    float statusTextY() { return static_cast<float>(Config::WINDOW_HEIGHT) / 2.f + STATUS_TEXT_OFFSET_FROM_CENTER; }
    float addressTextY() { return static_cast<float>(Config::WINDOW_HEIGHT) / 2.f + ADDRESS_TEXT_OFFSET_FROM_CENTER; }
    float menuTopY() { return static_cast<float>(Config::WINDOW_HEIGHT) / 2.f + MENU_TOP_OFFSET_FROM_CENTER; }

    // Centers `text` horizontally, keeping its top edge fixed - so a message
    // growing to two lines grows downward instead of jumping around.
    void centerHorizontally(sf::Text& text, float windowWidth)
    {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin({ bounds.position.x + bounds.size.x / 2.f, 0.f });
        text.setPosition({ windowWidth / 2.f, text.getPosition().y });
    }
}

NetworkLobbyState::NetworkLobbyState(sf::RenderWindow& window)
    : m_window(window)
    , m_background(AssetsManager::getInstance().getTexture("gameover_bg"))
    , m_statusText(AssetsManager::getInstance().getFont("Lilita"))
    , m_addressText(AssetsManager::getInstance().getFont("Lilita"))
    , m_menu({ static_cast<float>(Config::WINDOW_WIDTH) / 2.f, menuTopY() }, MENU_BUTTON_WIDTH, MENU_GAP)
{
    scaleBackgroundToWindow();

    m_statusText.setCharacterSize(24);
    m_statusText.setFillColor(sf::Color::White);
    m_statusText.setPosition({ 0.f, statusTextY() });

    m_addressText.setCharacterSize(28);
    m_addressText.setFillColor(sf::Color::Yellow);
    m_addressText.setPosition({ 0.f, addressTextY() });

    setPhase(Phase::ChooseRole);
}

void NetworkLobbyState::scaleBackgroundToWindow()
{
    SpriteUtils::scaleToFill(m_background, m_window.getSize());
}

void NetworkLobbyState::buildMenuForPhase(Phase phase)
{
    m_menu = Menu({ static_cast<float>(Config::WINDOW_WIDTH) / 2.f, menuTopY() }, MENU_BUTTON_WIDTH, MENU_GAP);

    auto& am = AssetsManager::getInstance();
    const sf::Texture& buttonTexture = am.getTexture("GenericButton");
    const sf::Font& font = am.getFont("Lilita");

    switch (phase)
    {
    case Phase::ChooseRole:
        m_menu.addButton(buttonTexture, [this]() { startHosting(); }, font, "Host Game");
        m_menu.addButton(buttonTexture, [this]() { startJoining(); }, font, "Join Game");
        m_menu.addButton(buttonTexture, [this]() { transitionTo(); }, font, "Back");
        break;

    case Phase::EnterAddress:
        m_menu.addButton(buttonTexture, [this]() { attemptConnect(); }, font, "Connect");
        m_menu.addButton(buttonTexture, [this]() { transitionTo(); }, font, "Back");
        break;

    case Phase::Hosting:
    case Phase::Connecting:
        break; // no buttons - just the status text, until the connection finishes on its own
    }
}

void NetworkLobbyState::setPhase(Phase phase)
{
    m_phase = phase;
    buildMenuForPhase(phase);

    switch (phase)
    {
    case Phase::ChooseRole:
        m_statusText.setString("Play against someone on your network");
        break;
    case Phase::Hosting:
        m_statusText.setString("Your address: " + NetworkConnection::getLocalAddress() +
            "\nShare it with your opponent - waiting for them to connect...");
        break;
    case Phase::EnterAddress:
        m_statusText.setString("Enter the host's address, then press Enter or Connect:");
        break;
    case Phase::Connecting:
        m_statusText.setString("Connecting to " + m_addressInput + "...");
        break;
    }

    centerHorizontally(m_statusText, static_cast<float>(Config::WINDOW_WIDTH));
    m_addressText.setString(m_addressInput);
    centerHorizontally(m_addressText, static_cast<float>(Config::WINDOW_WIDTH));
}

void NetworkLobbyState::startHosting()
{
    m_connection = std::make_unique<NetworkConnection>();
    if (!m_connection->startHosting(PORT))
    {
        m_statusText.setString("Couldn't start hosting: " + m_connection->getLastError());
        centerHorizontally(m_statusText, static_cast<float>(Config::WINDOW_WIDTH));
        m_connection.reset();
        return;
    }
    m_pendingPhase = Phase::Hosting; // not setPhase() directly - see applyPendingPhase
}

void NetworkLobbyState::startJoining()
{
    m_pendingPhase = Phase::EnterAddress;
}

void NetworkLobbyState::attemptConnect()
{
    if (m_addressInput.empty()) return;

    m_connection = std::make_unique<NetworkConnection>();
    if (!m_connection->connectToHost(m_addressInput, PORT))
    {
        m_statusText.setString("Couldn't connect: " + m_connection->getLastError());
        centerHorizontally(m_statusText, static_cast<float>(Config::WINDOW_WIDTH));
        m_connection.reset();
        return;
    }
    m_pendingPhase = Phase::Connecting;
}

void NetworkLobbyState::applyPendingPhase()
{
    if (!m_pendingPhase) return;
    Phase phase = *m_pendingPhase;
    m_pendingPhase.reset();
    setPhase(phase);
}

void NetworkLobbyState::enterGame(PlayerSide localSide)
{
    transitionTo(std::make_unique<GameplayState>(m_window, std::move(m_connection), localSide));
}

void NetworkLobbyState::update(sf::Time /*deltaTime*/)
{
    applyPendingPhase();

    if (!m_connection) return;

    m_connection->update();

    switch (m_phase)
    {
    case Phase::Hosting:
        if (m_connection->isConnected())
            enterGame(PlayerSide::Left); // host is always Left, by convention
        break;

    case Phase::Connecting:
        if (m_connection->isConnected())
            enterGame(PlayerSide::Right); // joiner is always Right
        break;

    default:
        break;
    }
}

void NetworkLobbyState::draw(sf::RenderWindow& window) const
{
    window.draw(m_background);
    window.draw(m_statusText);

    if (m_phase == Phase::EnterAddress)
        window.draw(m_addressText);

    m_menu.draw(window);
}

void NetworkLobbyState::handleEvent(const sf::Event& event)
{
    m_menu.handleEvent(event);

    if (m_phase != Phase::EnterAddress) return;

    if (const auto* text = event.getIf<sf::Event::TextEntered>())
    {
        // Digits and '.' only - this is always an IPv4 address, nothing else.
        if ((text->unicode >= '0' && text->unicode <= '9') || text->unicode == '.')
        {
            if (m_addressInput.size() < 15) // "255.255.255.255"
                m_addressInput += static_cast<char>(text->unicode);
            m_addressText.setString(m_addressInput);
            centerHorizontally(m_addressText, static_cast<float>(Config::WINDOW_WIDTH));
        }
    }
    else if (const auto* key = event.getIf<sf::Event::KeyPressed>())
    {
        // Backspace only does something once there's actually a character to remove.
        if (key->code == sf::Keyboard::Key::Backspace && !m_addressInput.empty())
        {
            m_addressInput.pop_back();
            m_addressText.setString(m_addressInput);
            centerHorizontally(m_addressText, static_cast<float>(Config::WINDOW_WIDTH));
        }
        else if (key->code == sf::Keyboard::Key::Enter)
        {
            attemptConnect();
        }
    }
}

MusicTrack NetworkLobbyState::desiredMusicTrack() const
{
    return MusicTrack::Menu;
}
