#pragma once
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

// A LAN TCP connection to exactly one peer (this game is always 2 players).
// Wraps Winsock directly since all this needs is connect/send/receive bytes.
// Non-blocking throughout - the single-threaded game loop redraws every
// frame, so a blocking recv() would freeze the window; update() is the only
// function that touches the socket, called once per frame. Every message is
// framed with a 4-byte length prefix since TCP is a byte stream, not a
// message stream.
class NetworkConnection
{
public:
    NetworkConnection();
    ~NetworkConnection();

    NetworkConnection(const NetworkConnection&) = delete;
    NetworkConnection& operator=(const NetworkConnection&) = delete;

    // Host role: opens a socket listening on port. False means the port
    // itself couldn't be opened - check getLastError().
    bool startHosting(unsigned short port);

    // Host role: non-blocking - true exactly once a peer has connected. False every frame before that.
    bool acceptConnection();

    // Join role: starts connecting to hostAddress:port. Non-blocking -
    // check isConnected() each frame afterward.
    bool connectToHost(const std::string& hostAddress, unsigned short port);

    bool isConnected() const { return m_connected; }

    // This machine's own LAN IP, shown to the hosting player to read out to
    // whoever's joining. Empty string if it couldn't be determined.
    static std::string getLocalAddress();

    // Frames data with its own length prefix and queues it - actually sent incrementally by update().
    void sendMessage(const std::vector<std::uint8_t>& data);

    // Drains whatever the socket has ready right now, in both directions. Never blocks. Called once per frame.
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

    // Outgoing: queued by sendMessage() but not yet handed to the OS socket buffer.
    std::deque<std::uint8_t> m_outgoing;

    // Incoming: the length prefix of the message being received, and however
    // many of its payload bytes have arrived so far.
    std::vector<std::uint8_t> m_lengthPrefixBuffer;
    std::uint32_t m_incomingMessageLength = 0;
    bool m_haveIncomingLength = false;
    std::vector<std::uint8_t> m_incomingPayload;

    // Fully-received messages waiting for popMessage() - a queue since
    // several small messages could arrive within the same frame.
    std::deque<std::vector<std::uint8_t>> m_completedIncoming;
};
