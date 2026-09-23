#pragma once
#include "State/State.h"
#include "NetworkConnection.h"
#include "Constants.h"
#include "Menu.h"
#include <optional>
#include <memory>
#include <string>

// The screen between GameModeState and an actual PlayerVsRemote
// GameplayState: lets the player choose to host or join a LAN match, then
// handles the connection handshake itself (accept/connect, then a shared
// board-generation seed - see Board::seedRng) before handing a fully
// connected NetworkConnection off to a freshly-built GameplayState.
// GameplayState itself never touches a socket or a raw connection attempt -
// by the time it exists, all of that is already done.
class NetworkLobbyState : public State
{
public:
    explicit NetworkLobbyState(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;

private:
    enum class Phase
    {
        ChooseRole,      // "Host Game" / "Join Game" / "Back"
        Hosting,         // listening, showing this machine's own LAN address
        EnterAddress,    // joiner types the host's address
        Connecting,      // joiner: connect() issued, waiting for it to complete
        ExchangingSeed,  // joiner only - waiting for the host's seed message
    };

    void scaleBackgroundToWindow();

    // Rebuilds m_menu with whichever buttons `phase` actually needs (Menu
    // has no remove-button operation, so a phase change just replaces it
    // outright) - the one place that decides which buttons show for which
    // phase, alongside setPhase's own per-phase status text.
    void buildMenuForPhase(Phase phase);

    void setPhase(Phase phase);
    void startHosting();
    void startJoining();
    void attemptConnect();

    // Both peers end up here once ready - the point where this State's own
    // job (getting a connected, seeded NetworkConnection) is done.
    void enterGame(PlayerSide localSide);

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Phase m_phase = Phase::ChooseRole;

    std::unique_ptr<NetworkConnection> m_connection;
    std::string m_addressInput;
    bool m_seedSent = false; // host only - sendMessage isn't itself idempotent

    sf::Text m_statusText;
    sf::Text m_addressText;
    Menu m_menu;
};
