#pragma once
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

// A LAN TCP connection to exactly one peer (this game is always 2 players -
// no lobby of many, no matchmaking server). Wraps Winsock directly rather
// than pulling in a networking library, since all this needs is "connect,
// send some bytes, receive some bytes" - see the design discussion this was
// built from for why LAN-only (no NAT traversal needed) and TCP (not UDP -
// losing/reordering a turn's actions would desync the two boards) were
// chosen.
//
// Non-blocking throughout, on purpose: this project's whole game loop
// (Controller::run) is single-threaded, redrawing every frame - a blocking
// recv() here would freeze the window. update() is the one function that
// actually touches the socket; call it once per frame (see
// GameplayState::update) and it always returns immediately, whether or not
// there was anything to do.
//
// TCP is a byte STREAM, not a message stream - what one side send()s in one
// call can arrive at the other side split across several recv() calls, or
// merged with the next message. Every message is framed with a 4-byte
// length prefix (see sendMessage/update) so the receiving side always knows
// exactly how many more bytes complete the message it's currently
// buffering, however many recv() calls that takes.
class NetworkConnection
{
public:
    NetworkConnection();
    ~NetworkConnection();

    NetworkConnection(const NetworkConnection&) = delete;
    NetworkConnection& operator=(const NetworkConnection&) = delete;

    // Host role: opens a socket listening on `port`. False means the port
    // itself couldn't be opened (already in use, etc.) - check
    // getLastError() for what actually went wrong.
    bool startHosting(unsigned short port);

    // Host role: non-blocking - true exactly once a peer has connected
    // (after which isConnected() is also true). False every frame before
    // that, including "still waiting" - not an error.
    bool acceptConnection();

    // Join role: starts connecting to hostAddress:port (e.g. "192.168.1.42").
    // Non-blocking - returns immediately; call isConnected() each frame
    // afterward to find out once it actually succeeds (or getLastError()
    // if the attempt fails outright).
    bool connectToHost(const std::string& hostAddress, unsigned short port);

    bool isConnected() const { return m_connected; }

    // This machine's own LAN IP address (e.g. "192.168.1.42") - shown to
    // the hosting player so they can read it out to whoever's joining (see
    // the design discussion: manual IP entry, not auto-discovery, is
    // today's scope). Empty string if it couldn't be determined.
    static std::string getLocalAddress();

    // Frames `data` with its own length prefix and queues it - actually
    // sent incrementally by update() below, never blocking here.
    void sendMessage(const std::vector<std::uint8_t>& data);

    // Drains whatever the socket has ready right now, in both directions -
    // continues sending anything still queued from sendMessage(), and
    // accumulates newly-arrived bytes into the current incoming message.
    // Never blocks. Called once per frame.
    void update();

    bool hasMessage() const { return !m_completedIncoming.empty(); }
    std::vector<std::uint8_t> popMessage();

    void disconnect();

    const std::string& getLastError() const { return m_lastError; }

private:
    bool setNonBlocking(std::uintptr_t sock);

    std::uintptr_t m_listenSocket = 0;
    std::uintptr_t m_socket = 0;
    bool m_connected = false;
    std::string m_lastError;

    // Outgoing: whatever sendMessage() has queued but update() hasn't
    // finished handing to the OS socket buffer yet.
    std::deque<std::uint8_t> m_outgoing;

    // Incoming: the 4-byte length prefix of the message currently being
    // received (once all 4 bytes of it have arrived), and however many of
    // its payload bytes have arrived so far.
    std::vector<std::uint8_t> m_lengthPrefixBuffer;
    std::uint32_t m_incomingMessageLength = 0;
    bool m_haveIncomingLength = false;
    std::vector<std::uint8_t> m_incomingPayload;

    // Fully-received messages, waiting for popMessage() - a queue (not a
    // single slot) since several small turn-action messages could arrive
    // within the same frame on a fast LAN.
    std::deque<std::vector<std::uint8_t>> m_completedIncoming;
};
