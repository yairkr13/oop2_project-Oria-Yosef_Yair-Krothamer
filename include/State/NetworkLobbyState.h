#pragma once
#include "State/State.h"
#include "NetworkConnection.h"
#include "Constants.h"
#include "Menu.h"
#include <optional>
#include <memory>
#include <string>

// The screen between GameModeState and a PlayerVsRemote GameplayState: lets
// the player host or join a LAN match, handles the connection handshake,
// then hands a connected NetworkConnection off to a fresh GameplayState.
// GameplayState itself never touches a socket or a raw connection attempt.
class NetworkLobbyState : public State
{
public:
    explicit NetworkLobbyState(sf::RenderWindow& window);

    void draw(sf::RenderWindow& window) const override;
    void update(sf::Time deltaTime) override;
    void handleEvent(const sf::Event& event) override;
    MusicTrack desiredMusicTrack() const override { return MusicTrack::Menu; }

private:
    enum class Phase
    {
        ChooseRole,      // "Host Game" / "Join Game" / "Back"
        Hosting,         // listening, showing this machine's own LAN address
        EnterAddress,    // joiner types the host's address
        Connecting,      // joiner: connect() issued, waiting for it to complete
    };

    void scaleBackgroundToWindow();

    // Rebuilds m_menu with the buttons `phase` needs - Menu has no
    // remove-button op, so a phase change just replaces it outright.
    void buildMenuForPhase(Phase phase);

    void setPhase(Phase phase);

    // Applies m_pendingPhase, once per frame at the start of update(). Never
    // called directly from a button callback, since setPhase() rebuilds
    // m_menu while Menu::handleEvent may still be iterating it.
    void applyPendingPhase();

    void startHosting();
    void startJoining();
    void attemptConnect();

    // Both peers end up here once ready - this state's job is done.
    void enterGame(PlayerSide localSide);

    sf::RenderWindow& m_window;
    sf::Sprite m_background;
    Phase m_phase = Phase::ChooseRole;
    std::optional<Phase> m_pendingPhase; // see applyPendingPhase

    std::unique_ptr<NetworkConnection> m_connection;
    std::string m_addressInput;

    sf::Text m_statusText;
    sf::Text m_addressText;
    Menu m_menu;
};
