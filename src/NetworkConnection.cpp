#include "NetworkConnection.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // windows.h's min/max macros would otherwise shadow std::min/std::max below
#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>
#include <cstring>

namespace
{
    // The one place this project's turn-action stream and any future
    // message both agree the length prefix is 4 bytes, big-endian ("network
    // byte order" - htonl/ntohl) - the conventional choice so two machines
    // with different native byte order (not a real concern between two
    // Windows PCs, but cheap to do correctly) still agree on it.
    constexpr std::size_t LENGTH_PREFIX_SIZE = 4;

    SOCKET toSocket(std::uintptr_t handle) { return static_cast<SOCKET>(handle); }
}

NetworkConnection::NetworkConnection()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        m_lastError = "WSAStartup failed";
}

NetworkConnection::~NetworkConnection()
{
    disconnect();
    WSACleanup();
}

bool NetworkConnection::setNonBlocking(std::uintptr_t sock)
{
    u_long mode = 1; // non-zero = non-blocking
    return ioctlsocket(toSocket(sock), FIONBIO, &mode) == 0;
}

bool NetworkConnection::startHosting(unsigned short port)
{
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET)
    {
        m_lastError = "socket() failed";
        return false;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(listenSock, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR)
    {
        m_lastError = "bind() failed - port " + std::to_string(port) + " may already be in use";
        closesocket(listenSock);
        return false;
    }

    if (listen(listenSock, 1) == SOCKET_ERROR) // backlog of 1 - this game is always exactly 2 players
    {
        m_lastError = "listen() failed";
        closesocket(listenSock);
        return false;
    }

    m_listenSocket = static_cast<std::uintptr_t>(listenSock);
    setNonBlocking(m_listenSocket);
    return true;
}

bool NetworkConnection::acceptConnection()
{
    if (m_connected || m_listenSocket == 0) return false;

    SOCKET accepted = accept(toSocket(m_listenSocket), nullptr, nullptr);
    if (accepted == INVALID_SOCKET)
        return false; // WSAEWOULDBLOCK (no one's connected yet) - not an error, just not yet

    m_socket = static_cast<std::uintptr_t>(accepted);
    setNonBlocking(m_socket);
    m_connected = true;

    // The listening socket's only job was accepting this one connection -
    // this game never accepts a second peer.
    closesocket(toSocket(m_listenSocket));
    m_listenSocket = 0;
    return true;
}

bool NetworkConnection::connectToHost(const std::string& hostAddress, unsigned short port)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        m_lastError = "socket() failed";
        return false;
    }

    setNonBlocking(static_cast<std::uintptr_t>(sock));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (inet_pton(AF_INET, hostAddress.c_str(), &address.sin_addr) != 1)
    {
        m_lastError = "'" + hostAddress + "' is not a valid IPv4 address";
        closesocket(sock);
        return false;
    }

    m_socket = static_cast<std::uintptr_t>(sock);

    // Non-blocking connect() returns immediately with WSAEWOULDBLOCK - the
    // attempt continues in the background; isConnected() only turns true
    // once update() below observes the socket has actually become writable
    // (the standard non-blocking-connect completion signal).
    int result = connect(sock, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    if (result == SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
    {
        m_lastError = "connect() failed";
        closesocket(sock);
        m_socket = 0;
        return false;
    }

    return true;
}

bool NetworkConnection::isConnected() const
{
    return m_connected;
}

std::string NetworkConnection::getLocalAddress()
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0)
        return "";

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* result = nullptr;
    if (getaddrinfo(hostname, nullptr, &hints, &result) != 0 || !result)
        return "";

    char addressText[INET_ADDRSTRLEN] = {};
    auto* ipv4 = reinterpret_cast<sockaddr_in*>(result->ai_addr);
    inet_ntop(AF_INET, &ipv4->sin_addr, addressText, sizeof(addressText));
    freeaddrinfo(result);

    return addressText;
}

void NetworkConnection::sendMessage(const std::vector<std::uint8_t>& data)
{
    std::uint32_t length = htonl(static_cast<std::uint32_t>(data.size()));
    const auto* lengthBytes = reinterpret_cast<const std::uint8_t*>(&length);

    m_outgoing.insert(m_outgoing.end(), lengthBytes, lengthBytes + LENGTH_PREFIX_SIZE);
    m_outgoing.insert(m_outgoing.end(), data.begin(), data.end());
}

void NetworkConnection::update()
{
    if (m_listenSocket != 0)
        acceptConnection(); // still hosting, waiting for the peer to join

    if (m_socket == 0) return;

    if (!m_connected)
    {
        // A non-blocking connect() has finished (successfully or not) once
        // the socket becomes writable - the standard way to detect this
        // without blocking.
        fd_set writeSet;
        FD_ZERO(&writeSet);
        FD_SET(toSocket(m_socket), &writeSet);
        timeval noWait{ 0, 0 };
        if (select(0, nullptr, &writeSet, nullptr, &noWait) > 0)
            m_connected = true;
        else
            return; // still connecting
    }

    // --- Send whatever's queued, as much as the socket accepts right now ---
    while (!m_outgoing.empty())
    {
        std::vector<std::uint8_t> chunk(m_outgoing.begin(), m_outgoing.end());
        int sent = send(toSocket(m_socket), reinterpret_cast<const char*>(chunk.data()),
            static_cast<int>(chunk.size()), 0);
        if (sent <= 0) break; // WSAEWOULDBLOCK (socket buffer full right now) - try again next frame
        m_outgoing.erase(m_outgoing.begin(), m_outgoing.begin() + sent);
    }

    // --- Receive whatever's arrived, accumulating into the current message ---
    char buffer[4096];
    int received;
    while ((received = recv(toSocket(m_socket), buffer, sizeof(buffer), 0)) > 0)
    {
        std::size_t offset = 0;
        while (offset < static_cast<std::size_t>(received))
        {
            if (!m_haveIncomingLength)
            {
                std::size_t need = LENGTH_PREFIX_SIZE - m_lengthPrefixBuffer.size();
                std::size_t take = std::min(need, static_cast<std::size_t>(received) - offset);
                m_lengthPrefixBuffer.insert(m_lengthPrefixBuffer.end(), buffer + offset, buffer + offset + take);
                offset += take;

                if (m_lengthPrefixBuffer.size() == LENGTH_PREFIX_SIZE)
                {
                    std::uint32_t networkLength;
                    std::memcpy(&networkLength, m_lengthPrefixBuffer.data(), LENGTH_PREFIX_SIZE);
                    m_incomingMessageLength = ntohl(networkLength);
                    m_haveIncomingLength = true;
                    m_lengthPrefixBuffer.clear();
                    m_incomingPayload.reserve(m_incomingMessageLength);
                }
            }
            else
            {
                std::size_t need = m_incomingMessageLength - m_incomingPayload.size();
                std::size_t take = std::min(need, static_cast<std::size_t>(received) - offset);
                m_incomingPayload.insert(m_incomingPayload.end(), buffer + offset, buffer + offset + take);
                offset += take;

                if (m_incomingPayload.size() == m_incomingMessageLength)
                {
                    m_completedIncoming.push_back(std::move(m_incomingPayload));
                    m_incomingPayload.clear();
                    m_haveIncomingLength = false;
                }
            }
        }
    }
    // received == 0 -> peer closed the connection cleanly; a negative
    // result with WSAEWOULDBLOCK just means nothing's arrived this frame -
    // neither needs anything different done here right now.
}

bool NetworkConnection::hasMessage() const
{
    return !m_completedIncoming.empty();
}

std::vector<std::uint8_t> NetworkConnection::popMessage()
{
    if (m_completedIncoming.empty()) return {};
    std::vector<std::uint8_t> message = std::move(m_completedIncoming.front());
    m_completedIncoming.pop_front();
    return message;
}

void NetworkConnection::disconnect()
{
    if (m_socket != 0)
    {
        closesocket(toSocket(m_socket));
        m_socket = 0;
    }
    if (m_listenSocket != 0)
    {
        closesocket(toSocket(m_listenSocket));
        m_listenSocket = 0;
    }
    m_connected = false;
}

const std::string& NetworkConnection::getLastError() const
{
    return m_lastError;
}
