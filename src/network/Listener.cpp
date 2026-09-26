#include "Listener.hpp"
#include "Client.hpp"
#include "PollLoop.hpp"
#include <stdexcept>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

// Forward declaration – PollLoop is the concrete IPoll in use
extern PollLoop* g_loop;

Listener::Listener(const std::string& host, int port, const ServerConfig* cfg)
    : fd_(-1), config_(cfg)
{
    setup(host, port);
}

Listener::~Listener() {
    // close(fd_) if fd_ >= 0
}

int Listener::fd() const { return fd_; }

const ServerConfig* Listener::config() const { return config_; }

void Listener::setup(const std::string& host, int port) {
    // 1. fd_ = socket(AF_INET, SOCK_STREAM, 0)
    //    throw on failure
    // 2. setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))
    // 3. fcntl(fd_, F_SETFL, O_NONBLOCK)
    // 4. Populate sockaddr_in with inet_aton(host) + htons(port)
    // 5. bind(fd_, ...) — throw std::runtime_error on failure with a message
    //    that includes host:port so the user knows which binding failed
    // 6. listen(fd_, SOMAXCONN)
    (void)host; (void)port;
}

void Listener::onReadable(int fd) {
    // accept() the new connection (may return -1 if the POLLIN was spurious)
    // On success:
    //   1. fcntl(clientFd, F_SETFL, O_NONBLOCK)
    //   2. new Client(clientFd, config_)
    //   3. g_loop->add(clientFd, POLLIN, client)
    (void)fd;
}

void Listener::onWritable(int /*fd*/) {
    // Listening sockets are never registered for POLLOUT — no-op
}

void Listener::onError(int /*fd*/) {
    // Log the error; the listener fd is probably dead.
    // Remove from loop: g_loop->remove(fd_)
}
