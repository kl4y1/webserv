#pragma once
#include <vector>
#include <map>
#include <poll.h>
#include "IPoll.hpp"
#include "Config.hpp"

class Listener;
class Client;

// ── The single event loop ────────────────────────────────────────────────────
class PollLoop : public IPoll {
public:
    static const int TIMEOUT_SECS = 60; // idle client timeout

    PollLoop();
    ~PollLoop();

    // Build one Listener per ServerConfig; add them to the poll set.
    void init(const std::vector<ServerConfig>& configs);

    // Run until g_stop is set (SIGINT handler).
    void run();

    // ── IPoll implementation ──────────────────────────────────────────────
    void add(int fd, short events, IPollHandler* handler);
    void modify(int fd, short events);
    void remove(int fd);

private:
    std::vector<struct pollfd>   pollfds_;   // passed directly to poll()
    std::map<int, IPollHandler*> handlers_;  // fd → handler (Client or Listener)
    std::vector<Listener*>       listeners_; // kept for cleanup
    std::vector<int>             toRemove_;  // fds to clean up after the event pass

    // Dispatch a single pollfd result to its handler
    void dispatch(struct pollfd& pfd);

    // Walk all clients; close any that have been idle > TIMEOUT_SECS
    void reapTimedOut();

    // Remove all fds queued in toRemove_ from pollfds_ and handlers_
    void compact();
};
