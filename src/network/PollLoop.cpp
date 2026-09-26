#include "PollLoop.hpp"
#include "Listener.hpp"
#include "Client.hpp"
#include <stdexcept>
#include <cerrno>
#include <poll.h>

// defined in main.cpp
extern volatile sig_atomic_t g_stop;

PollLoop::PollLoop() {}

PollLoop::~PollLoop() {
    // TODO: delete all Listener* in listeners_
    // TODO: delete all Client* IPollHandler* in handlers_ that are clients
}

void PollLoop::init(const std::vector<ServerConfig>& configs) {
    // For each ServerConfig in configs:
    //   1. new Listener(cfg.host, cfg.port, &cfg)
    //   2. add(listener->fd(), POLLIN, listener)
    //   3. push_back into listeners_
    (void)configs;
}

void PollLoop::run() {
    // Loop until g_stop != 0:
    //   1. call poll(pollfds_.data(), pollfds_.size(), 1000 /*ms*/)
    //      - if returns -1 and errno == EINTR, continue (signal received)
    //      - if returns -1 for any other reason, throw or break
    //   2. iterate pollfds_; for each with revents != 0, call dispatch(pfd)
    //   3. call reapTimedOut()
    //   4. call compact() to flush toRemove_ entries
}

void PollLoop::add(int fd, short events, IPollHandler* handler) {
    // Build a struct pollfd {fd, events, 0} and push to pollfds_
    // Insert handlers_[fd] = handler
    (void)fd; (void)events; (void)handler;
}

void PollLoop::modify(int fd, short events) {
    // Find the struct pollfd in pollfds_ whose .fd == fd
    // Set its .events = events
    (void)fd; (void)events;
}

void PollLoop::remove(int fd) {
    // Queue fd in toRemove_ (do not erase from pollfds_ mid-iteration)
    (void)fd;
}

void PollLoop::dispatch(struct pollfd& pfd) {
    // Look up handlers_[pfd.fd]
    // If POLLIN  | POLLHUP | POLLERR: call handler->onReadable(pfd.fd)
    // If POLLOUT: call handler->onWritable(pfd.fd)
    // If POLLERR | POLLHUP only (no POLLIN): call handler->onError(pfd.fd)
    (void)pfd;
}

void PollLoop::reapTimedOut() {
    // Iterate handlers_; dynamic_cast to Client*
    // If client->isTimedOut(TIMEOUT_SECS): call client->onError(fd)
    //   which will close and queue the fd into toRemove_
}

void PollLoop::compact() {
    // For each fd in toRemove_:
    //   1. erase from handlers_
    //   2. find and set that pollfd's .fd to -1 so poll() ignores it
    //      (or erase it; if erasing, be careful not to invalidate iterators
    //       — build a new vector excluding those fds)
    // Clear toRemove_
}
